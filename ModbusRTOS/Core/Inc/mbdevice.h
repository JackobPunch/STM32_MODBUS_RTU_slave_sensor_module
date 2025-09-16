/**
 * @file    mbdevice.h
 * @brief   Modbus device interface header
 * @author  Generated for RTOS version
 */

#ifndef _MBDEVICE_H_
#define _MBDEVICE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* Modbus device configuration structure */
    typedef struct
    {
        uint8_t devaddr;                                /* Slave address */
        uint8_t coils;                                  /* Number of coils (not used) */
        uint8_t discrete;                               /* Number of discrete inputs (not used) */
        void *device;                                   /* Device pointer (not used) */
        int (*send)(void *, const uint8_t *, uint16_t); /* Send function */
        uint16_t (*read)(uint32_t);                     /* Read register function */
        uint16_t (*write)(uint32_t, uint16_t);          /* Write register function */
        uint8_t *sendbuf;                               /* Send buffer */
        uint16_t sendbuf_sz;                            /* Send buffer size */
        uint8_t *recvbuf;                               /* Receive buffer */
        uint16_t recvbuf_sz;                            /* Receive buffer size */
    } Modbus_Conf_t;

    /* Modbus response types */
    typedef enum
    {
        MBUS_RESPONSE_OK = 0,
        MBUS_RESPONSE_ILLEGAL_FUNCTION = 1,
        MBUS_RESPONSE_ILLEGAL_DATA_ADDRESS = 2,
        MBUS_RESPONSE_ILLEGAL_DATA_VALUE = 3,
        MBUS_RESPONSE_SLAVE_DEVICE_FAILURE = 4,
        MBUS_RESPONSE_ACKNOWLEDGE = 5,
        MBUS_RESPONSE_SLAVE_DEVICE_BUSY = 6,
        MBUS_RESPONSE_NEGATIVE_ACKNOWLEDGE = 7,
        MBUS_RESPONSE_MEMORY_PARITY_ERROR = 8
    } Modbus_ResponseType;

    /* Modbus context handle */
    typedef int mbus_t;

    /* Function prototypes */
    uint16_t Modbus_Device_Read(uint32_t logical_address);
    uint16_t Modbus_Device_Write(uint32_t logical_address, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif /* _MBDEVICE_H_ */