/**
 * @file    modbus.c
 * @brief   Modbus RTU library implementation
 * @author  Generated for RTOS version
 */

#include "modbus.h"
#include "mbutils.h"
#include "main.h" // For GPIO functions
#include <string.h>
#include <stdio.h>

/* Modbus context structure */
typedef struct
{
    Modbus_Conf_t conf;
    uint8_t open;
    Modbus_StateType state;
    uint16_t crc16;
    uint8_t header[8];
    uint8_t header_len;
} _stmodbus_context_t;

/* Global variables */
_stmodbus_context_t g_mbusContext[STMODBUS_COUNT_CONTEXT];
Modbus_ResponseType g_userError = MBUS_RESPONSE_OK;

/* Private function prototypes */
static mbus_status_t mbus_send_error(mbus_t mb_context, Modbus_ResponseType error);
static void Modbus_ProcessReadRegisters(_stmodbus_context_t *ctx);
static void Modbus_ProcessReadInputRegisters(_stmodbus_context_t *ctx);
static void Modbus_ProcessWriteRegister(_stmodbus_context_t *ctx);
static void Modbus_ProcessWriteRegisters(_stmodbus_context_t *ctx);
static void Modbus_SendErrorResponse(_stmodbus_context_t *ctx, uint8_t error_code);

/* Exported functions */

/**
 * @brief  Open new Modbus context
 * @param  pconf: Modbus configuration
 * @retval Context handle or MBUS_ERROR
 */
mbus_t mbus_open(Modbus_Conf_t *pconf)
{
    mbus_t context;

    for (context = 0; context < STMODBUS_COUNT_CONTEXT; context++)
    {
        if (g_mbusContext[context].open == 0)
        {
            break;
        }
    }

    if (context == STMODBUS_COUNT_CONTEXT)
        return (mbus_t)MBUS_ERROR;

    // Clear context
    memset(&g_mbusContext[context], 0, sizeof(_stmodbus_context_t));

    // Copy config
    memcpy((void *)&g_mbusContext[context].conf, (void *)pconf, sizeof(Modbus_Conf_t));

    g_mbusContext[context].open = 1;
    return context;
}

/**
 * @brief  Close Modbus context
 * @param  context: Modbus context
 * @retval Status
 */
mbus_status_t mbus_close(mbus_t context)
{
    if (context >= STMODBUS_COUNT_CONTEXT)
        return MBUS_ERROR;

    g_mbusContext[context].open = 0;
    return MBUS_OK;
}

/**
 * @brief  Flush Modbus context
 * @param  context: Modbus context
 * @retval Status
 */
mbus_status_t mbus_flush(const mbus_t context)
{
    g_mbusContext[context].crc16 = 0xFFFF;
    g_mbusContext[context].state = MBUS_STATE_IDLE;
    return MBUS_OK;
}

/**
 * @brief  Process received Modbus data
 * @param  mb_context: Modbus context
 * @param  data: Received byte
 * @retval Status
 */
mbus_status_t mbus_poll(mbus_t mb_context, uint8_t data)
{
    _stmodbus_context_t *ctx = &g_mbusContext[mb_context];

    // Update CRC
    ctx->crc16 = mbutils_crc16_update(ctx->crc16, data);

    // Process based on current state
    switch (ctx->state)
    {
    case MBUS_STATE_IDLE:
        // Check if this is our slave address
        if (data == ctx->conf.devaddr)
        {
            ctx->state = MBUS_STATE_RECEIVING;
            ctx->header[0] = data;
            ctx->header_len = 1;
            // printf("MODBUS: Valid slave address detected: 0x%02X\n", data);
        }
        break;

    case MBUS_STATE_RECEIVING:
        // Continue receiving frame
        if (ctx->header_len < sizeof(ctx->header))
        {
            ctx->header[ctx->header_len++] = data;

            // Check if we have a complete Modbus frame (minimum 8 bytes for function 3/4/6/16)
            if (ctx->header_len >= 8)
            {
                // Validate CRC (last 2 bytes)
                uint16_t received_crc = (ctx->header[ctx->header_len - 1] << 8) | ctx->header[ctx->header_len - 2];
                uint16_t calculated_crc = mbutils_crc16(ctx->header, ctx->header_len - 2);

                if (received_crc == calculated_crc)
                {
                    // printf("MODBUS: Valid frame received, function: 0x%02X\n", ctx->header[1]);

                    // Process the frame
                    ctx->state = MBUS_STATE_PROCESSING;

                    // Handle different function codes
                    switch (ctx->header[1])
                    {
                    case MBUS_FUNC_READ_REGS: // Function 3 - Read Holding Registers
                        Modbus_ProcessReadRegisters(ctx);
                        break;

                    case MBUS_FUNC_READ_INPUT_REGS: // Function 4 - Read Input Registers
                        Modbus_ProcessReadInputRegisters(ctx);
                        break;

                    case MBUS_FUNC_WRITE_REG: // Function 6 - Write Single Register
                        Modbus_ProcessWriteRegister(ctx);
                        break;

                    case MBUS_FUNC_WRITE_REGS: // Function 16 - Write Multiple Registers
                        Modbus_ProcessWriteRegisters(ctx);
                        break;

                    default:
                        // printf("MODBUS: Unsupported function code: 0x%02X\n", ctx->header[1]);
                        Modbus_SendErrorResponse(ctx, MBUS_RESPONSE_ILLEGAL_FUNCTION);
                        break;
                    }

                    // Reset state for next frame
                    ctx->state = MBUS_STATE_IDLE;
                    ctx->header_len = 0;
                }
                else
                {
                    // printf("MODBUS: CRC error - received: 0x%04X, calculated: 0x%04X\n", received_crc, calculated_crc);
                    // Reset state on CRC error
                    ctx->state = MBUS_STATE_IDLE;
                    ctx->header_len = 0;
                }
            }
        }
        else
        {
            // printf("MODBUS: Frame buffer overflow\n");
            // Buffer overflow, reset
            ctx->state = MBUS_STATE_IDLE;
            ctx->header_len = 0;
        }
        break;

    default:
        // Reset to idle on unknown state
        ctx->state = MBUS_STATE_IDLE;
        ctx->header_len = 0;
        break;
    }

    return MBUS_OK;
}

/**
 * @brief  Send Modbus response
 * @param  mb_context: Modbus context
 * @param  response: Response type
 * @retval Status
 */
mbus_status_t mbus_response(mbus_t mb_context, Modbus_ResponseType response)
{
    if (response != MBUS_RESPONSE_OK)
    {
        return mbus_send_error(mb_context, response);
    }

    return MBUS_ERROR;
}

/**
 * @brief  Set error response
 * @param  error: Error type
 * @retval Error code
 */
uint16_t mbus_error(Modbus_ResponseType error)
{
    g_userError = error;
    return 0;
}

/* Private functions */

/**
 * @brief  Send Modbus error response
 * @param  mb_context: Modbus context
 * @param  error: Error type
 * @retval Status
 */
static mbus_status_t mbus_send_error(mbus_t mb_context, Modbus_ResponseType error)
{
    _stmodbus_context_t *ctx = &g_mbusContext[mb_context];
    uint8_t response[5];
    uint16_t crc;

    response[0] = ctx->conf.devaddr;
    response[1] = ctx->header[1] | 0x80; // Function code with error bit
    response[2] = error;

    crc = mbutils_crc16(response, 3);
    response[3] = crc & 0xFF;
    response[4] = (crc >> 8) & 0xFF;

    if (ctx->conf.send)
    {
        ctx->conf.send(NULL, response, 5);
    }

    return MBUS_OK;
}

/* Modbus frame processing functions */

void Modbus_ProcessReadRegisters(_stmodbus_context_t *ctx)
{
    // Extract register address and count from frame
    uint16_t start_addr = (ctx->header[2] << 8) | ctx->header[3];
    uint16_t reg_count = (ctx->header[4] << 8) | ctx->header[5];

    // printf("MODBUS: Read %d registers starting at %d\n", reg_count, start_addr);

    // Validate parameters
    if (reg_count == 0 || reg_count > 125)
    {
        Modbus_SendErrorResponse(ctx, MBUS_RESPONSE_ILLEGAL_DATA_VALUE);
        return;
    }

    // Build response
    uint8_t response[256];
    response[0] = ctx->conf.devaddr; // Slave address
    response[1] = ctx->header[1];    // Function code
    response[2] = reg_count * 2;     // Byte count

    // Read registers
    for (uint16_t i = 0; i < reg_count; i++)
    {
        uint16_t reg_value = ctx->conf.read(start_addr + i + 40001);
        response[3 + (i * 2)] = reg_value >> 8;
        response[3 + (i * 2) + 1] = reg_value & 0xFF;
    }

    // Calculate and append CRC
    uint16_t crc = mbutils_crc16(response, 3 + (reg_count * 2));
    response[3 + (reg_count * 2)] = crc & 0xFF;
    response[3 + (reg_count * 2) + 1] = (crc >> 8) & 0xFF;

    // Send response
    ctx->conf.send(NULL, response, 5 + (reg_count * 2));
}

void Modbus_ProcessReadInputRegisters(_stmodbus_context_t *ctx)
{
    // For now, treat input registers the same as holding registers
    Modbus_ProcessReadRegisters(ctx);
}

void Modbus_ProcessWriteRegister(_stmodbus_context_t *ctx)
{
    // Extract register address and value from frame
    uint16_t reg_addr = (ctx->header[2] << 8) | ctx->header[3];
    uint16_t reg_value = (ctx->header[4] << 8) | ctx->header[5];

    // printf("MODBUS: Write register %d with value %d\n", reg_addr, reg_value);

    // Write register
    ctx->conf.write(reg_addr + 40001, reg_value);

    // Send echo response (same as request for function 6)
    uint8_t response[8];
    memcpy(response, ctx->header, 6); // Copy first 6 bytes

    // Calculate and append CRC
    uint16_t crc = mbutils_crc16(response, 6);
    response[6] = crc & 0xFF;
    response[7] = (crc >> 8) & 0xFF;

    // Send response
    ctx->conf.send(NULL, response, 8);
}

void Modbus_ProcessWriteRegisters(_stmodbus_context_t *ctx)
{
    // Extract parameters from frame
    uint16_t start_addr = (ctx->header[2] << 8) | ctx->header[3];
    uint16_t reg_count = (ctx->header[4] << 8) | ctx->header[5];
    uint8_t byte_count = ctx->header[6];

    // printf("MODBUS: Write %d registers starting at %d\n", reg_count, start_addr);

    // Validate parameters
    if (reg_count == 0 || reg_count > 123 || byte_count != reg_count * 2)
    {
        Modbus_SendErrorResponse(ctx, MBUS_RESPONSE_ILLEGAL_DATA_VALUE);
        return;
    }

    // Write registers
    for (uint16_t i = 0; i < reg_count; i++)
    {
        uint16_t reg_value = (ctx->header[7 + (i * 2)] << 8) | ctx->header[8 + (i * 2)];
        ctx->conf.write(start_addr + i + 40001, reg_value);
    }

    // Send response (address and count)
    uint8_t response[8];
    response[0] = ctx->conf.devaddr;
    response[1] = ctx->header[1];
    response[2] = ctx->header[2];
    response[3] = ctx->header[3];
    response[4] = ctx->header[4];
    response[5] = ctx->header[5];

    // Calculate and append CRC
    uint16_t crc = mbutils_crc16(response, 6);
    response[6] = crc & 0xFF;
    response[7] = (crc >> 8) & 0xFF;

    // Send response
    ctx->conf.send(NULL, response, 8);
}

void Modbus_SendErrorResponse(_stmodbus_context_t *ctx, uint8_t error_code)
{
    uint8_t response[5];
    response[0] = ctx->conf.devaddr;
    response[1] = ctx->header[1] | 0x80; // Function code with error bit
    response[2] = error_code;

    // Calculate and append CRC
    uint16_t crc = mbutils_crc16(response, 3);
    response[3] = crc & 0xFF;
    response[4] = (crc >> 8) & 0xFF;

    // Send error response
    ctx->conf.send(NULL, response, 5);
}