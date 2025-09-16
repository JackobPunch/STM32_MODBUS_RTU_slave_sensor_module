/**
 * @file    modbus_init.h
 * @brief   Modbus initialization header
 * @author  Generated for RTOS version
 */

#ifndef __MODBUS_INIT_H
#define __MODBUS_INIT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "modbus.h"

    /* Exported variables */
    extern uint8_t modbus_rx_buffer[MODBUS_RX_BUFFER_SIZE];
    extern uint8_t modbus_tx_buffer[MODBUS_TX_BUFFER_SIZE];

    /* Exported functions */
    void Modbus_Init(void);
    void Modbus_Process(void);
    mbus_t Modbus_GetContext(void);

#ifdef __cplusplus
}
#endif

#endif /* __MODBUS_INIT_H */