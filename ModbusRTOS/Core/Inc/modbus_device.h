/**
 * @file    modbus_device.h
 * @brief   Modbus device-specific functions header
 * @author  Generated for RTOS version
 */

#ifndef __MODBUS_DEVICE_H
#define __MODBUS_DEVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "modbus_conf.h"

    /* Exported variables */
    extern uint16_t device_registers[MODBUS_DEVICE_REGISTERS];

    /* Exported functions */
    uint16_t Modbus_Device_Read(uint32_t logical_address);
    uint16_t Modbus_Device_Write(uint32_t logical_address, uint16_t value);
    void Modbus_Device_SetRegister(uint8_t index, uint16_t value);
    uint16_t Modbus_Device_GetRegister(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* __MODBUS_DEVICE_H */