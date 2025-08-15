#include "scd30.h"
#include <string.h>

#include <stdio.h>

// SCD30 commands
#define SCD30_CMD_START_MEASUREMENT 0x0010
#define SCD30_CMD_READ_MEASUREMENT 0x0300

#define SCD30_CMD_GET_DATA_READY 0x0202

static uint8_t tx_buf[5];
static uint8_t rx_buf[18];

// Helper: CRC8 for SCD30

static uint8_t SCD30_CalcCRC(const uint8_t *data, uint8_t len)
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

// Poll SCD30 data ready status
HAL_StatusTypeDef SCD30_DataReady(I2C_HandleTypeDef *hi2c, uint8_t *ready)
{
    uint8_t tx[2] = {(SCD30_CMD_GET_DATA_READY >> 8) & 0xFF, SCD30_CMD_GET_DATA_READY & 0xFF};
    uint8_t rx[3];
    HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(hi2c, SCD30_I2C_ADDR, tx, 2, 100);
    if (res != HAL_OK)
        return res;
    res = HAL_I2C_Master_Receive(hi2c, SCD30_I2C_ADDR, rx, 3, 100);
    if (res != HAL_OK)
        return res;
    // rx[0] and rx[1] are the status, rx[2] is CRC
    if (SCD30_CalcCRC(rx, 2) != rx[2])
        return HAL_ERROR;
    *ready = (rx[0] << 8) | rx[1];
    return HAL_OK;
}

HAL_StatusTypeDef SCD30_StartMeasurement(I2C_HandleTypeDef *hi2c)
{
    tx_buf[0] = (SCD30_CMD_START_MEASUREMENT >> 8) & 0xFF;
    tx_buf[1] = SCD30_CMD_START_MEASUREMENT & 0xFF;
    tx_buf[2] = 0x00; // 0x0000: ambient pressure
    tx_buf[3] = 0x00;
    tx_buf[4] = SCD30_CalcCRC(&tx_buf[2], 2);
    return HAL_I2C_Master_Transmit(hi2c, SCD30_I2C_ADDR, tx_buf, 5, 100);
}

HAL_StatusTypeDef SCD30_ReadMeasurement(I2C_HandleTypeDef *hi2c, float *co2, float *temperature, float *humidity)
{
    tx_buf[0] = (SCD30_CMD_READ_MEASUREMENT >> 8) & 0xFF;
    tx_buf[1] = SCD30_CMD_READ_MEASUREMENT & 0xFF;
    HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(hi2c, SCD30_I2C_ADDR, tx_buf, 2, 100);
    if (res != HAL_OK)
        return res;
    res = HAL_I2C_Master_Receive(hi2c, SCD30_I2C_ADDR, rx_buf, 18, 200);
    if (res != HAL_OK)
        return res;
    // Debug: print raw received bytes
    printf("Raw SCD30 bytes: ");
    for (int k = 0; k < 18; k++)
        printf("%02X ", rx_buf[k]);
    printf("\r\n");
    // Parse 3 float values (each: 4 bytes + CRC per 2 bytes)
    uint8_t data[12];
    for (int i = 0, j = 0; i < 18; i += 6)
    {
        // Check CRC for each 2 bytes
        if (SCD30_CalcCRC(&rx_buf[i], 2) != rx_buf[i + 2] || SCD30_CalcCRC(&rx_buf[i + 3], 2) != rx_buf[i + 5])
            return HAL_ERROR;
        data[j++] = rx_buf[i];
        data[j++] = rx_buf[i + 1];
        data[j++] = rx_buf[i + 3];
        data[j++] = rx_buf[i + 4];
    }
    // Convert to float
    // SCD30 sends big-endian, STM32 uses little-endian
    uint8_t co2_bytes[4] = {data[3], data[2], data[1], data[0]};
    uint8_t temperature_bytes[4] = {data[7], data[6], data[5], data[4]};
    uint8_t humidity_bytes[4] = {data[11], data[10], data[9], data[8]};

    memcpy(co2, co2_bytes, 4);
    memcpy(temperature, temperature_bytes, 4);
    memcpy(humidity, humidity_bytes, 4);
    return HAL_OK;
}
