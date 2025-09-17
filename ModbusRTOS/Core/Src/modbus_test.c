/**
 * @file    modbus_test.c
 * @brief   Modbus testing functions
 * @author  Generated for RTOS version
 */

#include "modbus_test.h"
#include "modbus_device.h"
#include "cmsis_os.h"

/* Private variables */
static uint32_t test_counter = 0;

/* Exported functions */

/**
 * @brief  Initialize Modbus test functions
 * @param  None
 * @retval None
 */
void Modbus_Test_Init(void)
{
    // Set initial test values - start with recognizable patterns
    Modbus_Device_SetRegister(0, 1000);    // 40001: Test sensor 1 - start at 1000
    Modbus_Device_SetRegister(1, 1100);    // 40002: Test sensor 2 - start at 1100
    Modbus_Device_SetRegister(2, 1200);    // 40003: Test sensor 3 - start at 1200
    Modbus_Device_SetRegister(3, 1300);    // 40004: Test sensor 4 - start at 1300
    Modbus_Device_SetRegister(4, 1400);    // 40005: Test sensor 5 - start at 1400
    Modbus_Device_SetRegister(5, 1500);    // 40006: Test sensor 6 - start at 1500
    Modbus_Device_SetRegister(6, 1600);    // 40007: Test sensor 7 - start at 1600
    Modbus_Device_SetRegister(7, 1700);    // 40008: Test sensor 8 - start at 1700
    Modbus_Device_SetRegister(8, 1800);    // 40009: Test sensor 9 - start at 1800
    Modbus_Device_SetRegister(9, 1900);    // 40010: Test sensor 10 - start at 1900
    Modbus_Device_SetRegister(10, 2000);   // 40011: Status register
    Modbus_Device_SetRegister(11, 0x0100); // 40012: Firmware version 1.0
}

/**
 * @brief  Update test values periodically
 * @param  None
 * @retval None
 */
void Modbus_Test_Update(void)
{
    test_counter++;

    // Update test sensor values every 50 cycles (faster updates)
    if (test_counter % 50 == 0)
    {
        static uint16_t test_value = 1000; // Start from 1000
        test_value += 10;                  // Increment by 10 each time for more noticeable change

        // Update first 10 registers with rising pattern
        Modbus_Device_SetRegister(0, test_value);      // 40001: Base value
        Modbus_Device_SetRegister(1, test_value + 10); // 40002: Base + 10
        Modbus_Device_SetRegister(2, test_value + 20); // 40003: Base + 20
        Modbus_Device_SetRegister(3, test_value + 30); // 40004: Base + 30
        Modbus_Device_SetRegister(4, test_value + 40); // 40005: Base + 40
        Modbus_Device_SetRegister(5, test_value + 50); // 40006: Base + 50
        Modbus_Device_SetRegister(6, test_value + 60); // 40007: Base + 60
        Modbus_Device_SetRegister(7, test_value + 70); // 40008: Base + 70
        Modbus_Device_SetRegister(8, test_value + 80); // 40009: Base + 80
        Modbus_Device_SetRegister(9, test_value + 90); // 40010: Base + 90
    }

    // Update status register (only when needed to avoid conflicts)
    if (test_counter % 200 == 0) // Every 20 seconds
    {
        uint16_t status = 0x0001;              // Bit 0: Ready
        status |= 0x0002;                      // Bit 1: Data updated
        Modbus_Device_SetRegister(10, status); // Use register 10 for status (40011)
    }
}