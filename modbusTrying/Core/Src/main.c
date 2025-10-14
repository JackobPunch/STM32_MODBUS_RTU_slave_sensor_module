/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus.h"
#include "mbutils.h"
#include "modbus_init.h"
#include "modbus_device.h"
#include "uart_callbacks.h"
#include "modbus_test.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* SOFTWARE WATCHDOG */
static volatile uint32_t software_watchdog_counter = 0;
#define SOFTWARE_WATCHDOG_TIMEOUT_MS 10000 // 10 seconds timeout

/* MODBUS RECOVERY SYSTEM */
static volatile uint32_t modbus_last_activity = 0;
static volatile uint32_t modbus_recovery_counter = 0;
static volatile uint32_t modbus_error_count = 0;
static volatile uint32_t modbus_last_error_reset = 0;
#define MODBUS_RECOVERY_TIMEOUT_MS 2500   // 2.5 seconds without activity triggers recovery (faster for 1s scan rate)
#define MODBUS_RECOVERY_INTERVAL_MS 10000 // Force recovery every 10 seconds (more aggressive preventive recovery)
#define MODBUS_ERROR_THRESHOLD 3          // Trigger recovery after 3 consecutive errors
#define MODBUS_ERROR_WINDOW_MS 5000       // Reset error count every 5 seconds

/* USER CODE BEGIN PV */
extern uint16_t device_registers[20]; // Access to device registers for testing
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void SoftwareWatchdog_Init(void);
void SoftwareWatchdog_Refresh(void);
void ModbusRecovery_Init(void);
void ModbusRecovery_Update(void);
void ModbusRecovery_ResetState(void);
void ModbusRecovery_MarkActivity(void);
void ModbusRecovery_MarkError(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */
  // initialise_monitor_handles(); // Removed - causes issues in standalone mode
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // Debug: Init step 1 - GPIO

  MX_DMA_Init();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // Debug: Init step 2 - DMA

  SoftwareWatchdog_Init(); // Initialize software watchdog for system protection

  MX_USART1_UART_Init();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); // Debug: Init step 3 - UART

  /* USER CODE BEGIN 2 */

  // printf("Starting Modbus RTU Slave...\n"); // Removed to prevent timing delays

  // Initialize UART callbacks
  UART_Callbacks_Init();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); // Debug: Init step 4 - UART callbacks

  // Initialize Modbus RTU slave
  Modbus_Init();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // Debug: Init step 5 - Modbus

  // Debug: Check array contents at startup (commented out to avoid timing delays)
  // printf("Array check: [0]=0x%04X [1]=0x%04X [12]=0x%04X [13]=0x%04X\n",
  //        Modbus_Device_GetRegister(0), Modbus_Device_GetRegister(1),
  //        Modbus_Device_GetRegister(12), Modbus_Device_GetRegister(13));

  // Initialize test functions
  Modbus_Test_Init();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Debug: Init step 6 - Test functions

  // Initialize Modbus recovery system
  ModbusRecovery_Init();

  // printf("Test functions initialized\n"); // Removed

  // printf("System ready! Slave address: 0x01\n"); // Removed

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // static uint32_t test_counter = 0; // Commented out - unused when test updates are disabled

  while (1)
  {
    /* USER CODE BEGIN 3 */

    // Refresh software watchdog to prevent system reset
    SoftwareWatchdog_Refresh();

    // Optimized heartbeat LED - much less frequent to minimize interference
    static uint32_t heartbeat_counter = 0;
    heartbeat_counter++;
    if (heartbeat_counter >= 200) // 200 * 50ms = 10 seconds (very slow heartbeat)
    {
      heartbeat_counter = 0;
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    }

    // Process Modbus first (highest priority)
    Modbus_Process();

    // Check and handle Modbus recovery if needed
    ModbusRecovery_Update();

    // Update sensor values periodically (lower priority)
    Modbus_Device_UpdateSensors();

    // Update test values (lowest priority)
    Modbus_Test_Update(); // Test: Modify register values every 3 seconds to verify callbacks are working
    // DISABLED temporarily to test if this was causing timeouts
    /*
    test_counter++;
    if (test_counter % 30 == 0) // Every 30 * 100ms = 3 seconds
    {
      // Add 1 to each register value
      for (int i = 0; i < 20; i++)
      {
        device_registers[i] += 1;
      }
    }
    */

    // Very short delay - prioritize Modbus communication responsiveness
    HAL_Delay(10); // Reduced to 10ms for maximum Modbus responsiveness

    /* USER CODE END 3 */
  }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  // Configure USART1 clock source
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_RS485Ex_Init(&huart1, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}

/**
 * @brief Software Watchdog Initialization Function
 * @param None
 * @retval None
 */
void SoftwareWatchdog_Init(void)
{
  software_watchdog_counter = HAL_GetTick();
}

/**
 * @brief Refresh Software Watchdog
 * @param None
 * @retval None
 */
void SoftwareWatchdog_Refresh(void)
{
  uint32_t current_tick = HAL_GetTick();

  // Check if timeout occurred (accounting for tick overflow)
  if ((current_tick - software_watchdog_counter) > SOFTWARE_WATCHDOG_TIMEOUT_MS)
  {
    // System appears to be stuck - perform reset
    NVIC_SystemReset();
  }

  // Refresh the watchdog timer
  software_watchdog_counter = current_tick;
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3 PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief Initialize Modbus Recovery System
 * @param None
 * @retval None
 */
void ModbusRecovery_Init(void)
{
  uint32_t current_tick = HAL_GetTick();
  modbus_last_activity = current_tick;
  modbus_recovery_counter = current_tick;
  modbus_error_count = 0;
  modbus_last_error_reset = current_tick;
}

/**
 * @brief Mark Modbus Activity (call from UART callback)
 * @param None
 * @retval None
 */
void ModbusRecovery_MarkActivity(void)
{
  modbus_last_activity = HAL_GetTick();
}

/**
 * @brief Update Modbus Recovery System
 * @param None
 * @retval None
 */
void ModbusRecovery_Update(void)
{
  uint32_t current_tick = HAL_GetTick();

  // Reset error count periodically
  if ((current_tick - modbus_last_error_reset) > MODBUS_ERROR_WINDOW_MS)
  {
    modbus_error_count = 0;
    modbus_last_error_reset = current_tick;
  }

  // Check for error threshold (immediate recovery if too many errors)
  if (modbus_error_count >= MODBUS_ERROR_THRESHOLD)
  {
    ModbusRecovery_ResetState();
    modbus_error_count = 0;
    modbus_last_error_reset = current_tick;
  }

  // Check for forced recovery interval (every 10 seconds)
  else if ((current_tick - modbus_recovery_counter) > MODBUS_RECOVERY_INTERVAL_MS)
  {
    ModbusRecovery_ResetState();
    modbus_recovery_counter = current_tick;
  }

  // Check for inactivity timeout (2.5 seconds without valid Modbus activity)
  else if ((current_tick - modbus_last_activity) > MODBUS_RECOVERY_TIMEOUT_MS)
  {
    ModbusRecovery_ResetState();
    modbus_last_activity = current_tick; // Reset timer to prevent immediate re-trigger
  }
}

/**
 * @brief Reset Modbus State Machine
 * @param None
 * @retval None
 */
void ModbusRecovery_ResetState(void)
{
  // Get Modbus context
  mbus_t modbus_ctx = Modbus_GetContext();

  if (modbus_ctx >= 0)
  {
    // Flush any pending data/state
    mbus_flush(modbus_ctx);

    // Restart DMA reception to clear any stuck states
    HAL_UART_DMAStop(&huart1);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, modbus_rx_buffer, sizeof(modbus_rx_buffer));

    // Clear any UART error flags
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    __HAL_UART_CLEAR_NEFLAG(&huart1);
    __HAL_UART_CLEAR_FEFLAG(&huart1);
    __HAL_UART_CLEAR_PEFLAG(&huart1);
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);

    // Brief pulse on PA7 for debugging (if available)
    // This helps identify when recovery occurs during testing
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_Delay(1); // Very brief pulse
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
  }
}

/**
 * @brief Mark Modbus Error (call when timeout/error detected)
 * @param None
 * @retval None
 */
void ModbusRecovery_MarkError(void)
{
  modbus_error_count++;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
