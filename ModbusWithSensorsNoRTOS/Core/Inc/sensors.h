/**
 * @file    sensors.h
 * @brief   Unified sensor interface for MQ2 gas sensors and SCD30 environmental sensor
 * @author  Integration for ModbusWithSensorsNoRTOS
 */

#ifndef __SENSORS_H
#define __SENSORS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define MQ2_NUM_CHANNELS 4         // 4 MQ2 sensors on ADC channels 0-3
#define SCD30_I2C_ADDR (0x61 << 1) // SCD30 I2C address (8-bit for HAL)

/* ADC channel mapping for MQ2 sensors */
#define MQ2_CH0_CHANNEL ADC_CHANNEL_1 // PA0 -> ADC1_IN1
#define MQ2_CH1_CHANNEL ADC_CHANNEL_2 // PA1 -> ADC1_IN2
#define MQ2_CH2_CHANNEL ADC_CHANNEL_3 // PA2 -> ADC1_IN3
#define MQ2_CH3_CHANNEL ADC_CHANNEL_4 // PA3 -> ADC1_IN4

/* GPIO pins for MQ2 digital outputs (DOUT) */
#define MQ2_CH0_DOUT_GPIO GPIOA
#define MQ2_CH0_DOUT_PIN GPIO_PIN_4 // PA4 -> MQ2 CH0 DOUT
#define MQ2_CH1_DOUT_GPIO GPIOA
#define MQ2_CH1_DOUT_PIN GPIO_PIN_5 // PA5 -> MQ2 CH1 DOUT
#define MQ2_CH2_DOUT_GPIO GPIOA
#define MQ2_CH2_DOUT_PIN GPIO_PIN_6 // PA6 -> MQ2 CH2 DOUT
#define MQ2_CH3_DOUT_GPIO GPIOA
#define MQ2_CH3_DOUT_PIN GPIO_PIN_7 // PA7 -> MQ2 CH3 DOUT

/* SCD30 command constants */
#define SCD30_CMD_START_MEASUREMENT 0x0010
#define SCD30_CMD_READ_MEASUREMENT 0x0300
#define SCD30_CMD_GET_DATA_READY 0x0202

    /* Exported types ------------------------------------------------------------*/
    typedef struct
    {
        uint16_t mq2_values[MQ2_NUM_CHANNELS];   // Raw ADC values (0-4095)
        uint16_t mq2_voltages[MQ2_NUM_CHANNELS]; // Voltage in millivolts
        uint8_t mq2_digital[MQ2_NUM_CHANNELS];   // Digital gas detection (1=gas detected, 0=no gas)
        float scd30_co2;                         // CO2 concentration in ppm
        float scd30_temperature;                 // Temperature in Celsius
        float scd30_humidity;                    // Relative humidity in %
        uint8_t scd30_data_ready;                // SCD30 data ready flag
        uint32_t last_update;                    // Timestamp of last sensor update
    } SensorData_t;

    /* Exported variables --------------------------------------------------------*/
    extern SensorData_t sensor_data;

    /* Exported function prototypes ----------------------------------------------*/

    /* MQ2 Gas Sensor Functions */
    HAL_StatusTypeDef MQ2_Init(void);
    HAL_StatusTypeDef MQ2_ReadChannel(uint8_t channel, uint16_t *adc_value, uint16_t *voltage_mv);
    HAL_StatusTypeDef MQ2_ReadDigitalChannel(uint8_t channel, uint8_t *gas_detected);
    HAL_StatusTypeDef MQ2_ReadAllChannels(void);

    /* SCD30 Environmental Sensor Functions */
    HAL_StatusTypeDef SCD30_Init(void);
    HAL_StatusTypeDef SCD30_StartMeasurement(void);
    HAL_StatusTypeDef SCD30_DataReady(uint8_t *ready);
    HAL_StatusTypeDef SCD30_ReadMeasurement(float *co2, float *temperature, float *humidity);
    HAL_StatusTypeDef SCD30_UpdateData(void);

    /* Unified Sensor Interface */
    void Sensors_Init(void);
    void Sensors_UpdateAll(void);
    uint16_t Sensors_GetMQ2Value(uint8_t channel);
    uint16_t Sensors_GetMQ2Voltage(uint8_t channel);
    uint8_t Sensors_GetMQ2Digital(uint8_t channel);
    float Sensors_GetSCD30_CO2(void);
    float Sensors_GetSCD30_Temperature(void);
    float Sensors_GetSCD30_Humidity(void);

    /* Utility Functions */
    uint8_t SCD30_CalcCRC(const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SENSORS_H */