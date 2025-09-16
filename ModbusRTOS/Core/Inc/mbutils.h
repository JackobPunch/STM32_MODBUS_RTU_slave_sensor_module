/**
 * @file    mbutils.h
 * @brief   Modbus utilities header
 * @author  Generated for RTOS version
 */

#ifndef _MBUTILS_H_
#define _MBUTILS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* CRC calculation functions */
    uint16_t mbutils_crc16(const uint8_t *buffer, uint16_t length);
    uint16_t mbutils_crc16_update(uint16_t crc, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* _MBUTILS_H_ */