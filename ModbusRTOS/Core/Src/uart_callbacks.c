/**
 * @file    uart_callbacks.c
 * @brief   UART callback functions for Modbus processing
 * @author  Generated for RTOS version
 */

#include "uart_callbacks.h"
#include "modbus_init.h"
#include "modbus.h"
#include <stdio.h>

/* Private variables */
extern UART_HandleTypeDef huart1;

/* Exported functions */

/**
 * @brief  Initialize UART callbacks
 * @param  None
 * @retval None
 */
void UART_Callbacks_Init(void)
{
    // UART callbacks are handled by HAL
}

/* HAL UART Callbacks */

/**
 * @brief  UART receive event callback
 * @param  huart: UART handle
 * @param  Size: Number of bytes received
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart1)
    {
        // printf("MODBUS: UART RX event, size: %d bytes\n", Size);

        // Get Modbus context
        mbus_t modbus_ctx = Modbus_GetContext();

        // DEBUG: Check if context is valid (should be 0)
        if (modbus_ctx != 0)
        {
            // printf("MODBUS: ERROR - Invalid Modbus context: %d\n", modbus_ctx);
            return;
        }

        // Process received Modbus data byte by byte
        for (uint16_t i = 0; i < Size; i++)
        {
            mbus_poll(modbus_ctx, modbus_rx_buffer[i]);
        }

        // Clear UART idle flag
        __HAL_UART_CLEAR_IDLEFLAG(huart);

        // Restart DMA reception
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, modbus_rx_buffer, sizeof(modbus_rx_buffer));
    }
}

/**
 * @brief  UART error callback
 * @param  huart: UART handle
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        // Clear error flags
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);

        // Restart DMA reception
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, modbus_rx_buffer, sizeof(modbus_rx_buffer));
    }
}

/**
 * @brief  UART abort callback
 * @param  huart: UART handle
 * @retval None
 */
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        // Restart DMA reception
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, modbus_rx_buffer, sizeof(modbus_rx_buffer));
    }
}