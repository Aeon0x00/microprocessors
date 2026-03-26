/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task.h"
#include <stdio.h>
#include "ssd1306.h"       // Добавить
#include "ssd1306_fonts.h" // Добавить
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
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart1;
I2C_HandleTypeDef hi2c1;
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task1ms */
osThreadId_t Task1msHandle;
const osThreadAttr_t Task1ms_attributes = {
  .name = "Task1ms",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task10ms */
osThreadId_t Task10msHandle;
const osThreadAttr_t Task10ms_attributes = {
  .name = "Task10ms",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task50ms */
osThreadId_t Task50msHandle;
const osThreadAttr_t Task50ms_attributes = {
  .stack_size = 512 * 4,
  .stack_size = 512 * 4,
  .name = "Task50ms",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for xQueueResistance */
osMessageQueueId_t xQueueResistanceHandle;
const osMessageQueueAttr_t xQueueResistance_attributes = {
  .name = "xQueueResistance"
};
/* Definitions for xQueueLux */
osMessageQueueId_t xQueueLuxHandle;
const osMessageQueueAttr_t xQueueLux_attributes = {
  .name = "xQueueLux"
};
/* USER CODE BEGIN PV */
volatile uint32_t task10msCnt = 0;
volatile uint32_t task1msCnt  = 0;
volatile uint32_t task50msCnt = 0;
volatile uint32_t luxValue = 0;
volatile uint32_t resistanceValue = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);
void Task1msHandler(void *argument);
void Task10msHandler(void *argument);
void Task50msHandler(void *argument);

/* USER CODE BEGIN PFP */

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
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init(); // Инициализация дисплея
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of xQueueResistance */
  xQueueResistanceHandle = osMessageQueueNew (5, 4, &xQueueResistance_attributes);

  /* creation of xQueueLux */
  xQueueLuxHandle = osMessageQueueNew (5, 4, &xQueueLux_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Task1ms */
  Task1msHandle = osThreadNew(Task1msHandler, NULL, &Task1ms_attributes);

  /* creation of Task10ms */
  Task10msHandle = osThreadNew(Task10msHandler, NULL, &Task10ms_attributes);

  /* creation of Task50ms */
  Task50msHandle = osThreadNew(Task50msHandler, NULL, &Task50ms_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Task1msHandler */
/**
* @brief Function implementing the Task1ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task1msHandler */
void Task1msHandler(void *argument)
{
  /* USER CODE BEGIN Task1msHandler */

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 10 / portTICK_PERIOD_MS; // тоже 10 мс
  xLastWakeTime = xTaskGetTickCount();

  for(;;)
  {
    task1msCnt++;

    uint32_t resistance = 0;

    if(osMessageQueueGet(xQueueResistanceHandle, &resistance, NULL, 5) == osOK)
      {
      uint32_t lux = 0;
      if(resistance > 0) // защита от деления на ноль
      {
          lux = 500000UL / resistance;
          luxValue = lux;
          resistanceValue = resistance;
      }

      osMessageQueuePut(xQueueLuxHandle, &lux, 0, 0);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }

  /* USER CODE END Task1msHandler */
}



/* USER CODE BEGIN Header_Task10msHandler */
/**
* @brief Function implementing the Task10ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task10msHandler */
void Task10msHandler(void *argument)
{
  /* USER CODE BEGIN Task10msHandler */


  TickType_t xLastWakeTime;

  const TickType_t xFrequency = 10 / portTICK_PERIOD_MS;

  // Запоминаем текущее время — это точка отсчёта для ПЕРВОГО запуска
  xLastWakeTime = xTaskGetTickCount();

  for(;;)
  {
    task10msCnt++; // счётчик для отладки — смотришь в Debug

    HAL_ADC_Start(&hadc1);

    HAL_ADC_PollForConversion(&hadc1, 10);


    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);

    // 4) Останавливаем АЦП
    HAL_ADC_Stop(&hadc1);


    uint32_t voltage_mV = (adcValue * 3300) / 4095;

    uint32_t resistance = 0;
    if(voltage_mV > 0) // защита: если voltage=0, то деление на ноль → краш
    {
      resistance = (10000UL * (3300 - voltage_mV)) / voltage_mV;
    }

    osMessageQueuePut(xQueueResistanceHandle, &resistance, 0, 0);

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }

  /* USER CODE END Task10msHandler */
}


/* USER CODE BEGIN Header_Task50msHandler */
/**
* @brief Function implementing the Task50ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task50msHandler */
void Task50msHandler(void *argument)
{
  /* USER CODE BEGIN Task50msHandler */
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 100 / portTICK_PERIOD_MS; // Для дисплея лучше 100мс

  char uartBuf[64];
  char dispBuf[20];

  for(;;)
  {
    uint32_t lux = 0;

    // Пытаемся взять значение из очереди
    if(osMessageQueueGet(xQueueLuxHandle, &lux, NULL, 10) == osOK)
    {
      // 1. Вывод в UART (ваш код)
      int len = snprintf(uartBuf, sizeof(uartBuf), "Lux: %lu\r\n", lux);
      HAL_UART_Transmit(&huart1, (uint8_t*)uartBuf, len, 10);

      // 2. Вывод на OLED дисплей
      ssd1306_Fill(Black); // Очищаем буфер экрана

      ssd1306_SetCursor(2, 0);
      ssd1306_WriteString("SMART SENSOR", Font_7x10, White);

      // Печатаем Lux крупно
      snprintf(dispBuf, sizeof(dispBuf), "%lu LUX", lux);
      ssd1306_SetCursor(10, 20);
      ssd1306_WriteString(dispBuf, Font_11x18, White);

      // Печатаем сопротивление мелко внизу
      snprintf(dispBuf, sizeof(dispBuf), "R: %lu Ohm", resistanceValue);
      ssd1306_SetCursor(2, 45);
      ssd1306_WriteString(dispBuf, Font_7x10, White);

      ssd1306_UpdateScreen(); // Отправляем данные на экран
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END Task50msHandler */
}



/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

/* Функция инициализации I2C1 */
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* Настройка ножек PB6 и PB7 для I2C */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}
