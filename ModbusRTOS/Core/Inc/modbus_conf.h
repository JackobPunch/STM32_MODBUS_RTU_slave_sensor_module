/**
 * @file    modbus_conf.h
 * @brief   Modbus configuration header
 * @author  Generated for RTOS version
 */

#ifndef __MODBUS_CONF_H
#define __MODBUS_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Modbus configuration */
#define STMODBUS_COUNT_CONTEXT 1
#define STMODBUS_USE_CRITICAL_SECTIONS 1

/* Critical section macros for FreeRTOS */
#define stmbEnterCriticalSection taskENTER_CRITICAL
#define stmbLeaveCriticalSection taskEXIT_CRITICAL

/* Modbus RTU configuration */
#define MODBUS_SLAVE_ADDRESS 0x01
#define MODBUS_BAUD_RATE 9600
#define MODBUS_DEVICE_REGISTERS 20

/* UART buffer sizes */
#define MODBUS_RX_BUFFER_SIZE 256
#define MODBUS_TX_BUFFER_SIZE 256

#ifdef __cplusplus
}
#endif

#endif /* __MODBUS_CONF_H */