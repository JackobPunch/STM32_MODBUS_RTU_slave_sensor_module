/**
 * @file    modbus_device.c
 * @brief   Modbus device-specific read/write functions with sensor integration
 * @author  Integration for ModbusWithSensorsNoRTOS
 */

#include "modbus_device.h"
#include "sensors.h"
#include <math.h>

/* Private variables ---------------------------------------------------------*/
// Device registers (Holding Registers - 4xxxx) - Mapped to sensor data
uint16_t device_registers[20] = {0};

/* Private function prototypes -----------------------------------------------*/
static uint16_t Float_To_ModbusRegister(float value, float scale);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Convert float to Modbus register with scaling
 * @param  value: Float value to convert
 * @param  scale: Scaling factor (e.g., 100 for 2 decimal places)
 * @retval Scaled integer value
 */
static uint16_t Float_To_ModbusRegister(float value, float scale)
{
    // Handle invalid values
    if (isnan(value) || isinf(value))
        return 0;

    // Scale and clamp to 16-bit range
    int32_t scaled = (int32_t)(value * scale);
    if (scaled < 0)
        scaled = 0;
    if (scaled > 65535)
        scaled = 65535;

    return (uint16_t)scaled;
}

/* Future use - convert Modbus register back to float (currently unused)
static float ModbusRegister_To_Float(uint16_t value, float scale)
{
    return (float)value / scale;
}
*/

/* Exported functions -------------------------------------------------------*/

/**
 * @brief  Get register value by index
 * @param  index: Register index (0-19)
 * @retval Register value
 */
uint16_t Modbus_Device_GetRegister(uint8_t index)
{
    if (index < 20)
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
    // Simple direct array access like the sample code
    if (logical_address >= 40001 && logical_address <= 40020)
    {
        uint16_t index = logical_address - 40001;
        uint16_t value = device_registers[index];

        // Debug: Show what we're returning for ALL registers
        // printf("READ: addr=%lu, index=%d, returning %d (0x%04X)\n", logical_address, index, value, value); // Removed to prevent timeouts

        return value;
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

        // Handle special write operations for configuration registers
        switch (logical_address)
        {
        case 40019: // System reset command
            if (value == 0x1234)
            {
                // System reset command
                NVIC_SystemReset();
            }
            break;

        case 40020: // Sensor force update command
            if (value == 0x5678)
            {
                // Force immediate sensor update
                Sensors_UpdateAll();
            }
            break;
        default:
            // Store value for other registers (allows testing)
            break;
        }

        // Store the value
        device_registers[index] = value;
        return value;
    }

    return 0; // Invalid address
}

/**
 * @brief  Update sensor values and map to Modbus registers
 * @param  None
 * @retval None
 */
void Modbus_Device_UpdateSensors(void)
{
    // Update all sensor readings
    Sensors_UpdateAll();

    // Map MQ2 sensor values to registers 40001-40004 (raw ADC values)
    device_registers[0] = Sensors_GetMQ2Value(0); // 40001: MQ2 CH0 ADC
    device_registers[1] = Sensors_GetMQ2Value(1); // 40002: MQ2 CH1 ADC
    device_registers[2] = Sensors_GetMQ2Value(2); // 40003: MQ2 CH2 ADC
    device_registers[3] = Sensors_GetMQ2Value(3); // 40004: MQ2 CH3 ADC

    // Map MQ2 voltage values to registers 40005-40008 (millivolts)
    device_registers[4] = Sensors_GetMQ2Voltage(0); // 40005: MQ2 CH0 mV
    device_registers[5] = Sensors_GetMQ2Voltage(1); // 40006: MQ2 CH1 mV
    device_registers[6] = Sensors_GetMQ2Voltage(2); // 40007: MQ2 CH2 mV
    device_registers[7] = Sensors_GetMQ2Voltage(3); // 40008: MQ2 CH3 mV

    // Map MQ2 digital gas detection to registers 40009-40012 (individual registers)
    device_registers[8] = Sensors_GetMQ2Digital(0) ? 1 : 0;  // 40009: MQ2 CH0 Gas Detection (1=gas, 0=no gas)
    device_registers[9] = Sensors_GetMQ2Digital(1) ? 1 : 0;  // 40010: MQ2 CH1 Gas Detection (1=gas, 0=no gas)
    device_registers[10] = Sensors_GetMQ2Digital(2) ? 1 : 0; // 40011: MQ2 CH2 Gas Detection (1=gas, 0=no gas)
    device_registers[11] = Sensors_GetMQ2Digital(3) ? 1 : 0; // 40012: MQ2 CH3 Gas Detection (1=gas, 0=no gas)

    // Map SCD30 values to registers 40013-40015 (scaled for Modbus)
    // CO2: Scale by 1 (direct ppm values, max 65535 ppm)
    device_registers[12] = Float_To_ModbusRegister(Sensors_GetSCD30_CO2(), 1.0f); // 40013: CO2 ppm

    // Temperature: Scale by 100 (2 decimal places) - direct value
    // 24.19°C should show as 2419
    float temperature = Sensors_GetSCD30_Temperature();
    if (temperature < -50.0f || temperature > 85.0f)
    {
        // Error condition - use special error value for invalid temperature
        device_registers[13] = 0xFFFF; // 65535 indicates sensor error
    }
    else
    {
        // Direct temperature × 100 (no stupid offset)
        device_registers[13] = (uint16_t)(temperature * 100.0f); // 40014: Temperature (°C × 100)
    }

    // Humidity: Scale by 100 (2 decimal places, 0.00 to 655.35%)
    device_registers[14] = Float_To_ModbusRegister(Sensors_GetSCD30_Humidity(), 100.0f); // 40015: Humidity (% * 100)

    // Status and diagnostic registers
    device_registers[15] = sensor_data.scd30_data_ready ? 1 : 0;       // 40016: SCD30 data ready flag
    device_registers[16] = (uint16_t)(HAL_GetTick() / 1000);           // 40017: System uptime (seconds)
    device_registers[17] = (uint16_t)(sensor_data.last_update / 1000); // 40018: Last sensor update (seconds)

    // Configuration/diagnostic registers (40015-40020) are handled by write function
}

/**
 * @brief  Set device register value (for internal use)
 * @param  index: Register index (0-19 for 40001-40020)
 * @param  value: Value to set
 * @retval None
 */
void Modbus_Device_SetRegister(uint8_t index, uint16_t value)
{
    if (index < 20)
    {
        device_registers[index] = value;
    }
}

/**
 * @brief  Debug function to print all register values
 * @param  None
 * @retval None
 */
void Modbus_Device_DebugArray(void)
{
    // Debug output can be enabled here if needed
    // printf("=== Modbus Registers ===\n");
    // for (int i = 0; i < 20; i++)
    // {
    //     printf("40%03d: %5d (0x%04X)\n", i+1, device_registers[i], device_registers[i]);
    // }
    // printf("========================\n");
}