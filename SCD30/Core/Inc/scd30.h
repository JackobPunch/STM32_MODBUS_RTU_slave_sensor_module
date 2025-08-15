#ifndef SCD30_H
#define SCD30_H

#include "stm32f3xx_hal.h"

#define SCD30_I2C_ADDR (0x61 << 1) // 8-bit address for HAL

HAL_StatusTypeDef SCD30_StartMeasurement(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef SCD30_ReadMeasurement(I2C_HandleTypeDef *hi2c, float *co2, float *temperature, float *humidity);

HAL_StatusTypeDef SCD30_DataReady(I2C_HandleTypeDef *hi2c, uint8_t *ready);

#endif // SCD30_H
