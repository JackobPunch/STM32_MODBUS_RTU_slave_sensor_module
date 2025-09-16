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

        // Handle special write operations
        switch (index)
        {
        case 14: // Configuration register 1 (40015)
            // Apply_Configuration1(value);
            break;

        case 15: // Configuration register 2 (40016)
            // Apply_Configuration2(value);
            break;

        case 16: // Calibration value 1 (40017)
            // Set_Calibration1(value);
            break;

        case 17: // Calibration value 2 (40018)
            // Set_Calibration2(value);
            break;

        case 18: // Test register (40019)
            // Handle test operations
            break;

        default:
            // Store value for other registers
            break;
        }

        // Store the value
        device_registers[index] = value;
        return value;
    }

    return 0; // Invalid address
}

/**
 * @brief  Update sensor values (call this periodically)
 * @param  None
 * @retval None
 */
void Modbus_Device_UpdateSensors(void)
{
    // Update sensor values - call this from RTOS task
    // device_registers[0] = Read_Sensor1_Value();  // 40001
    // device_registers[1] = Read_Sensor2_Value();  // 40002
    // device_registers[4] = Read_Temperature();    // 40005
    // device_registers[5] = Read_Humidity();       // 40006
    // etc.
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