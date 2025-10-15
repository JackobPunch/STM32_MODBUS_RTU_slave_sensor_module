/**
 * @file    sensors.c
 * @brief   Unified sensor interface implementation
 * @author  Integration for ModbusWithSensorsNoRTOS
 */

#include "sensors.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;

SensorData_t sensor_data = {0};

static uint8_t scd30_tx_buf[5];
static uint8_t scd30_rx_buf[18];

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef MQ2_ConfigureChannel(uint32_t channel);

/* MQ2 Gas Sensor Functions -------------------------------------------------*/

/**
 * @brief  Initialize MQ2 sensors
 * @param  None
 * @retval HAL status
 */
HAL_StatusTypeDef MQ2_Init(void)
{
    // ADC is already initialized by CubeMX
    // Just calibrate the ADC
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        return HAL_ERROR;
    }

    // Initialize sensor data
    memset(sensor_data.mq2_values, 0, sizeof(sensor_data.mq2_values));
    memset(sensor_data.mq2_voltages, 0, sizeof(sensor_data.mq2_voltages));
    memset(sensor_data.mq2_digital, 0, sizeof(sensor_data.mq2_digital));

    return HAL_OK;
}

/**
 * @brief  Configure ADC for specific channel
 * @param  channel: ADC channel to configure
 * @retval HAL status
 */
static HAL_StatusTypeDef MQ2_ConfigureChannel(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.SamplingTime = ADC_SAMPLETIME_181CYCLES_5;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;

    return HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/**
 * @brief  Read single MQ2 channel
 * @param  channel: Channel number (0-3)
 * @param  adc_value: Pointer to store raw ADC value
 * @param  voltage_mv: Pointer to store voltage in millivolts
 * @retval HAL status
 */
HAL_StatusTypeDef MQ2_ReadChannel(uint8_t channel, uint16_t *adc_value, uint16_t *voltage_mv)
{
    if (channel >= MQ2_NUM_CHANNELS)
        return HAL_ERROR;

    uint32_t adc_channel;
    switch (channel)
    {
    case 0:
        adc_channel = MQ2_CH0_CHANNEL;
        break;
    case 1:
        adc_channel = MQ2_CH1_CHANNEL;
        break;
    case 2:
        adc_channel = MQ2_CH2_CHANNEL;
        break;
    case 3:
        adc_channel = MQ2_CH3_CHANNEL;
        break;
    default:
        return HAL_ERROR;
    }

    // Configure ADC for this channel
    if (MQ2_ConfigureChannel(adc_channel) != HAL_OK)
        return HAL_ERROR;

    // Start ADC conversion
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
        return HAL_ERROR;

    // Wait for conversion to complete
    if (HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        return HAL_ERROR;
    }

    // Get ADC value
    uint32_t raw_value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    // Convert to 16-bit and calculate voltage
    *adc_value = (uint16_t)raw_value;
    *voltage_mv = (uint16_t)((raw_value * 3300) / 4095);

    return HAL_OK;
}

/**
 * @brief  Read MQ2 digital output (gas detection)
 * @param  channel: Channel number (0-3)
 * @param  gas_detected: Pointer to store gas detection status (1=gas, 0=no gas)
 * @retval HAL status
 */
HAL_StatusTypeDef MQ2_ReadDigitalChannel(uint8_t channel, uint8_t *gas_detected)
{
    if (channel >= MQ2_NUM_CHANNELS)
        return HAL_ERROR;

    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;

    // Select the appropriate GPIO port and pin for this channel
    switch (channel)
    {
    case 0:
        gpio_port = MQ2_CH0_DOUT_GPIO;
        gpio_pin = MQ2_CH0_DOUT_PIN;
        break;
    case 1:
        gpio_port = MQ2_CH1_DOUT_GPIO;
        gpio_pin = MQ2_CH1_DOUT_PIN;
        break;
    case 2:
        gpio_port = MQ2_CH2_DOUT_GPIO;
        gpio_pin = MQ2_CH2_DOUT_PIN;
        break;
    case 3:
        gpio_port = MQ2_CH3_DOUT_GPIO;
        gpio_pin = MQ2_CH3_DOUT_PIN;
        break;
    default:
        return HAL_ERROR;
    }

    // Read GPIO pin state
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);

    // MQ2 DOUT is typically active LOW (0 = gas detected, 1 = no gas)
    // Convert to logical high = gas detected for easier understanding
    *gas_detected = (pin_state == GPIO_PIN_RESET) ? 1 : 0;

    return HAL_OK;
}

/**
 * @brief  Read all MQ2 channels (analog + digital) and update sensor data
 * @param  None
 * @retval HAL status
 */
HAL_StatusTypeDef MQ2_ReadAllChannels(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    for (uint8_t i = 0; i < MQ2_NUM_CHANNELS; i++)
    {
        // Read analog values (ADC + voltage)
        if (MQ2_ReadChannel(i, &sensor_data.mq2_values[i], &sensor_data.mq2_voltages[i]) != HAL_OK)
        {
            status = HAL_ERROR;
        }

        // Read digital gas detection
        if (MQ2_ReadDigitalChannel(i, &sensor_data.mq2_digital[i]) != HAL_OK)
        {
            status = HAL_ERROR;
        }
    }

    return status;
}

/* SCD30 Environmental Sensor Functions -------------------------------------*/

/**
 * @brief  Calculate CRC8 for SCD30 communication
 * @param  data: Data bytes
 * @param  len: Number of bytes
 * @retval CRC8 value
 */
uint8_t SCD30_CalcCRC(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/**
 * @brief  Initialize SCD30 sensor
 * @param  None
 * @retval HAL status
 */
HAL_StatusTypeDef SCD30_Init(void)
{
    // I2C is already initialized by CubeMX
    // Initialize sensor data
    sensor_data.scd30_co2 = 0.0f;
    sensor_data.scd30_temperature = 0.0f;
    sensor_data.scd30_humidity = 0.0f;
    sensor_data.scd30_data_ready = 0;

    // Start continuous measurement
    return SCD30_StartMeasurement();
}

/**
 * @brief  Start SCD30 continuous measurement
 * @param  None
 * @retval HAL status
 */
HAL_StatusTypeDef SCD30_StartMeasurement(void)
{
    scd30_tx_buf[0] = (SCD30_CMD_START_MEASUREMENT >> 8) & 0xFF;
    scd30_tx_buf[1] = SCD30_CMD_START_MEASUREMENT & 0xFF;
    scd30_tx_buf[2] = 0x00; // 0x0000: ambient pressure compensation disabled
    scd30_tx_buf[3] = 0x00;
    scd30_tx_buf[4] = SCD30_CalcCRC(&scd30_tx_buf[2], 2);

    return HAL_I2C_Master_Transmit(&hi2c1, SCD30_I2C_ADDR, scd30_tx_buf, 5, 100);
}

/**
 * @brief  Check if SCD30 data is ready
 * @param  ready: Pointer to store ready status
 * @retval HAL status
 */
HAL_StatusTypeDef SCD30_DataReady(uint8_t *ready)
{
    uint8_t tx[2] = {(SCD30_CMD_GET_DATA_READY >> 8) & 0xFF, SCD30_CMD_GET_DATA_READY & 0xFF};
    uint8_t rx[3];

    HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(&hi2c1, SCD30_I2C_ADDR, tx, 2, 100);
    if (res != HAL_OK)
        return res;

    res = HAL_I2C_Master_Receive(&hi2c1, SCD30_I2C_ADDR, rx, 3, 100);
    if (res != HAL_OK)
        return res;

    // Verify CRC
    if (SCD30_CalcCRC(rx, 2) != rx[2])
        return HAL_ERROR;

    *ready = (rx[0] << 8) | rx[1];
    return HAL_OK;
}

/**
 * @brief  Read SCD30 measurement data
 * @param  co2: Pointer to store CO2 value (ppm)
 * @param  temperature: Pointer to store temperature (°C)
 * @param  humidity: Pointer to store humidity (%)
 * @retval HAL status
 */
HAL_StatusTypeDef SCD30_ReadMeasurement(float *co2, float *temperature, float *humidity)
{
    scd30_tx_buf[0] = (SCD30_CMD_READ_MEASUREMENT >> 8) & 0xFF;
    scd30_tx_buf[1] = SCD30_CMD_READ_MEASUREMENT & 0xFF;

    HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(&hi2c1, SCD30_I2C_ADDR, scd30_tx_buf, 2, 100);
    if (res != HAL_OK)
        return res;

    res = HAL_I2C_Master_Receive(&hi2c1, SCD30_I2C_ADDR, scd30_rx_buf, 18, 200);
    if (res != HAL_OK)
        return res;

    // Parse 3 float values (each: 4 bytes + CRC per 2 bytes)
    uint8_t data[12];
    for (int i = 0, j = 0; i < 18; i += 6)
    {
        // Check CRC for each 2-byte pair
        if (SCD30_CalcCRC(&scd30_rx_buf[i], 2) != scd30_rx_buf[i + 2] ||
            SCD30_CalcCRC(&scd30_rx_buf[i + 3], 2) != scd30_rx_buf[i + 5])
            return HAL_ERROR;

        data[j++] = scd30_rx_buf[i];
        data[j++] = scd30_rx_buf[i + 1];
        data[j++] = scd30_rx_buf[i + 3];
        data[j++] = scd30_rx_buf[i + 4];
    }

    // Convert from big-endian to little-endian and copy to float
    // SCD30 sends data in big-endian format, STM32 uses little-endian
    // So we reverse the byte order: [MSB, ..., LSB] -> [LSB, ..., MSB]
    uint8_t co2_bytes[4] = {data[3], data[2], data[1], data[0]};
    uint8_t temperature_bytes[4] = {data[7], data[6], data[5], data[4]};
    uint8_t humidity_bytes[4] = {data[11], data[10], data[9], data[8]};

    memcpy(co2, co2_bytes, 4);
    memcpy(temperature, temperature_bytes, 4);
    memcpy(humidity, humidity_bytes, 4);

    return HAL_OK;
}

/**
 * @brief  Update SCD30 data if ready
 * @param  None
 * @retval HAL status
 */
HAL_StatusTypeDef SCD30_UpdateData(void)
{
    // Check if data is ready
    if (SCD30_DataReady(&sensor_data.scd30_data_ready) != HAL_OK)
        return HAL_ERROR;

    // Read measurement if data is ready
    if (sensor_data.scd30_data_ready)
    {
        return SCD30_ReadMeasurement(&sensor_data.scd30_co2,
                                     &sensor_data.scd30_temperature,
                                     &sensor_data.scd30_humidity);
    }

    return HAL_OK;
}

/* Unified Sensor Interface -------------------------------------------------*/

/**
 * @brief  Initialize all sensors
 * @param  None
 * @retval None
 */
void Sensors_Init(void)
{
    // Initialize MQ2 sensors
    MQ2_Init();

    // Initialize SCD30 sensor (with delay for startup)
    HAL_Delay(1000); // SCD30 needs time to start up
    SCD30_Init();

    // Update timestamp
    sensor_data.last_update = HAL_GetTick();
}

/**
 * @brief  Update all sensor readings
 * @param  None
 * @retval None
 */
void Sensors_UpdateAll(void)
{
    // Update MQ2 sensors
    MQ2_ReadAllChannels();

    // Update SCD30 sensor
    SCD30_UpdateData();

    // Update timestamp
    sensor_data.last_update = HAL_GetTick();
}

/**
 * @brief  Get MQ2 raw ADC value
 * @param  channel: Channel number (0-3)
 * @retval Raw ADC value (0-4095)
 */
uint16_t Sensors_GetMQ2Value(uint8_t channel)
{
    if (channel >= MQ2_NUM_CHANNELS)
        return 0;
    return sensor_data.mq2_values[channel];
}

/**
 * @brief  Get MQ2 voltage in millivolts
 * @param  channel: Channel number (0-3)
 * @retval Voltage in millivolts
 */
uint16_t Sensors_GetMQ2Voltage(uint8_t channel)
{
    if (channel >= MQ2_NUM_CHANNELS)
        return 0;
    return sensor_data.mq2_voltages[channel];
}

/**
 * @brief  Get MQ2 digital gas detection status
 * @param  channel: Channel number (0-3)
 * @retval Gas detection status (1=gas detected, 0=no gas)
 */
uint8_t Sensors_GetMQ2Digital(uint8_t channel)
{
    if (channel >= MQ2_NUM_CHANNELS)
        return 0;
    return sensor_data.mq2_digital[channel];
}

/**
 * @brief  Get SCD30 CO2 concentration
 * @param  None
 * @retval CO2 concentration in ppm
 */
float Sensors_GetSCD30_CO2(void)
{
    return sensor_data.scd30_co2;
}

/**
 * @brief  Get SCD30 temperature
 * @param  None
 * @retval Temperature in Celsius
 */
float Sensors_GetSCD30_Temperature(void)
{
    return sensor_data.scd30_temperature;
}

/**
 * @brief  Get SCD30 humidity
 * @param  None
 * @retval Relative humidity in %
 */
float Sensors_GetSCD30_Humidity(void)
{
    return sensor_data.scd30_humidity;
}