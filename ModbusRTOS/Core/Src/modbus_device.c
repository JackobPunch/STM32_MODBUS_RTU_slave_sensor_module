/**
 * @file    modbus_device.c
 * @brief   Modbus device-specific read/write functions
 * @author  Generated for RTOS version
 */

#include "modbus_device.h"
#include "cmsis_os.h"

/* Private variables */
uint16_t device_registers[MODBUS_DEVICE_REGISTERS] = {
    20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
    10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

/* Private function prototypes */

/* Exported functions */

/**
 * @brief  Get register value by index
 * @param  index: Register index (0-19)
 * @retval Register value
 */
uint16_t Modbus_Device_GetRegister(uint8_t index)
{
    if (index < MODBUS_DEVICE_REGISTERS)
    {
        return device_registers[index];
    }
    return 0;
}

/**
 * @brief  Modbus device read callback
 * @param  logical_address: Modbus logical address (40001, 40002, etc.)
 * @retval Register value
 */
uint16_t Modbus_Device_Read(uint32_t logical_address)
{
    // Convert logical address to array index
    if (logical_address >= 40001 && logical_address <= 40020)
    {
        uint16_t index = logical_address - 40001;
        return device_registers[index];
    }

    return 0; // Invalid address
}

/**
 * @brief  Modbus device write callback
 * @param  logical_address: Modbus logical address (40001, 40002, etc.)
 * @param  value: Value to write
 * @retval Written value
 */
uint16_t Modbus_Device_Write(uint32_t logical_address, uint16_t value)
{
    // Convert logical address to array index
    if (logical_address >= 40001 && logical_address <= 40020)
    {
        uint16_t index = logical_address - 40001;

        // Store the value (simplified - no special cases needed for basic testing)
        device_registers[index] = value;
        return value;
    }

    return 0; // Invalid address
}

/**
 * @brief  Set device register value (for internal use)
 * @param  index: Register index (0-19 for 40001-40020)
 * @param  value: Value to set
 * @retval None
 */
void Modbus_Device_SetRegister(uint8_t index, uint16_t value)
{
    if (index < MODBUS_DEVICE_REGISTERS)
    {
        device_registers[index] = value;
    }
}