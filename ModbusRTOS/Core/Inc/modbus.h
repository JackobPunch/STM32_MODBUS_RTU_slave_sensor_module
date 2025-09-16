/**
 * @file    modbus.h
 * @brief   Modbus RTU library header
 * @author  Generated for RTOS version
 */

#ifndef _STMODBUS_DEFINE_H_
#define _STMODBUS_DEFINE_H_

#include "mbdevice.h"
#include "modbus_conf.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Critical section macros for RTOS */
#define stmbEnterCriticalSection taskENTER_CRITICAL
#define stmbLeaveCriticalSection taskEXIT_CRITICAL

    /* Modbus status codes */
    typedef enum
    {
        MBUS_OK = 0,
        MBUS_ERROR = -1
    } mbus_status_t;

    /* Modbus function codes */
    typedef enum
    {
        MBUS_FUNC_READ_COILS = 1,
        MBUS_FUNC_READ_DISCRETE = 2,
        MBUS_FUNC_READ_REGS = 3,
        MBUS_FUNC_READ_INPUT_REGS = 4,
        MBUS_FUNC_WRITE_COIL = 5,
        MBUS_FUNC_WRITE_REG = 6,
        MBUS_FUNC_READ_EXCEPT_STATUS = 7,
        MBUS_FUNC_DIAGNOSTICS = 8,
        MBUS_FUNC_GET_COMM_EVENT_COUNTER = 11,
        MBUS_FUNC_GET_COMM_EVENT_LOG = 12,
        MBUS_FUNC_WRITE_COILS = 15,
        MBUS_FUNC_WRITE_REGS = 16,
        MBUS_FUNC_READ_SLAVE_ID = 17,
        MBUS_FUNC_READ_FILE_RECORD = 20,
        MBUS_FUNC_WRITE_FILE_RECORD = 21,
        MBUS_FUNC_READ_WRITE_MASK_REGS = 22,
        MBUS_FUNC_READ_WRITE_REGS = 23,
        MBUS_FUNC_READ_FIFO_QUEUE = 24,
        MBUS_FUNC_READ_DEVICE_ID = 43,
        MBUS_FUNC_EXCEPTION = 0x81,
    } Modbus_ConnectFuncType;

    /* Modbus state machine states */
    typedef enum
    {
        MBUS_STATE_IDLE = 0,
        MBUS_STATE_RECEIVING = 1,
        MBUS_STATE_PROCESSING = 2,
        MBUS_STATE_SENDING = 3
    } Modbus_StateType;

    /* Function prototypes */
    mbus_t mbus_open(Modbus_Conf_t *pconf);
    mbus_status_t mbus_close(mbus_t context);
    mbus_status_t mbus_flush(const mbus_t context);
    mbus_status_t mbus_poll(mbus_t mb_context, uint8_t data);
    mbus_status_t mbus_response(mbus_t mb_context, Modbus_ResponseType response);
    uint16_t mbus_error(Modbus_ResponseType error);

#ifdef __cplusplus
}
#endif

#endif /* _STMODBUS_DEFINE_H_ */