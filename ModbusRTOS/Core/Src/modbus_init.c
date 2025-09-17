/**
 * @file    modbus_init.c
 * @brief   Modbus initialization and processing
 * @author  Generated for RTOS version
 */

#include "modbus_init.h"
#include "main.h"
#include "modbus_device.h"
#include "mbutils.h"
#include <stdio.h>

/* Private variables */
extern UART_HandleTypeDef huart1;
static mbus_t modbus_context;
static Modbus_Conf_t modbus_config;
uint8_t modbus_rx_buffer[MODBUS_RX_BUFFER_SIZE];
uint8_t modbus_tx_buffer[MODBUS_TX_BUFFER_SIZE];

/* Private function prototypes */
static int Modbus_SendData(void *context, const uint8_t *data, const uint16_t size);

/* Exported functions */

/**
 * @brief  Initialize Modbus RTU slave
 * @param  None
 * @retval None
 */
void Modbus_Init(void)
{
    // Configure Modbus
    modbus_config.devaddr = MODBUS_SLAVE_ADDRESS;
    modbus_config.coils = 0;
    modbus_config.discrete = 0;
    modbus_config.device = NULL;
    modbus_config.send = Modbus_SendData;
    modbus_config.read = Modbus_Device_Read;
    modbus_config.write = Modbus_Device_Write;
    modbus_config.sendbuf = modbus_tx_buffer;
    modbus_config.sendbuf_sz = sizeof(modbus_tx_buffer);
    modbus_config.recvbuf = modbus_rx_buffer;
    modbus_config.recvbuf_sz = sizeof(modbus_rx_buffer);

    // Initialize Modbus context
    modbus_context = mbus_open(&modbus_config);

    // Note: device_registers are already initialized in modbus_device.c
    // with values: 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1

    // Start UART DMA reception
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, modbus_rx_buffer, sizeof(modbus_rx_buffer));

    // Enable UART idle line interrupt
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    // Modbus initialization complete
}

/**
 * @brief  Process Modbus communication
 * @param  None
 * @retval None
 */
void Modbus_Process(void)
{
    // This function is called from RTOS task
    // Processing is handled in UART callback
}

/* Private functions */

/**
 * @brief  Modbus send function
 * @param  context: Modbus context
 * @param  data: Data to send
 * @param  size: Size of data
 * @retval Number of bytes sent
 */
static int Modbus_SendData(void *context, const uint8_t *data, const uint16_t size)
{
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t *)data, size, 1000);

    if (status == HAL_OK)
    {
        return size;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  Get Modbus context
 * @param  None
 * @retval Modbus context
 */
mbus_t Modbus_GetContext(void)
{
    return modbus_context;
}