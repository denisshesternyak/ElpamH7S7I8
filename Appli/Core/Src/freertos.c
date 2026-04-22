/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>

//#include "lcd_menu.h"
//#include "audio_types.h"
//#include "rs232.h"
//#include "logger.h"
#include "app_freertos.h"
#include "events.h"
#include "fatfs.h"
#include "sdfs.h"
#include "usart.h"
#include "logger.h"
#include "lcd_menu.h"

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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for UartTask */
osThreadId_t UartTaskHandle;
const osThreadAttr_t UartTask_attributes = {
    .name = "UartTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for AudioTask */
osThreadId_t AudioTaskHandle;
const osThreadAttr_t AudioTask_attributes = {
    .name = "AudioTask",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for LcdTask */
osThreadId_t LcdTaskHandle;
const osThreadAttr_t LcdTask_attributes = {
    .name = "LcdTask",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for KeyboardTask */
osThreadId_t KeyboardTaskHandle;
const osThreadAttr_t KeyboardTask_attributes = {
    .name = "KeyboardTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for LoggerTask */
osThreadId_t LoggerTaskHandle;
const osThreadAttr_t LoggerTask_attributes = {
    .name = "LoggerTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for SDTask */
osThreadId_t SDTaskHandle;
const osThreadAttr_t SDTask_attributes = {
    .name = "SDTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for xLCDQueue */
osMessageQueueId_t xLCDQueueHandle;
const osMessageQueueAttr_t xLCDQueue_attributes = { .name = "xLCDQueue" };
/* Definitions for xAudioQueue */
osMessageQueueId_t xAudioQueueHandle;
const osMessageQueueAttr_t xAudioQueue_attributes = { .name = "xAudioQueue" };
/* Definitions for xUartQueue */
osMessageQueueId_t xUartQueueHandle;
const osMessageQueueAttr_t xUartQueue_attributes = { .name = "xUartQueue" };
/* Definitions for xLoggerQueue */
osMessageQueueId_t xLoggerQueueHandle;
const osMessageQueueAttr_t xLoggerQueue_attributes = { .name = "xLoggerQueue" };
/* Definitions for LoggerMutex */
osMutexId_t LoggerMutexHandle;
const osMutexAttr_t LoggerMutex_attributes = { .name = "LoggerMutex" };
/* Definitions for LoggerBinarySem */
osSemaphoreId_t LoggerBinarySemHandle;
const osSemaphoreAttr_t LoggerBinarySem_attributes = { .name = "LoggerBinarySem" };
/* Definitions for LcdBinarySem */
osSemaphoreId_t LcdBinarySemHandle;
const osSemaphoreAttr_t LcdBinarySem_attributes = { .name = "LcdBinarySem" };
/* Definitions for KeyboardEvent */
osEventFlagsId_t KeyboardEventHandle;
const osEventFlagsAttr_t KeyboardEvent_attributes = { .name = "KeyboardEvent" };
/* Definitions for SDEvent */
osEventFlagsId_t SDEventHandle;
const osEventFlagsAttr_t SDEvent_attributes = { .name = "SDEvent" };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask (void *argument);
void StartUartTask (void *argument);
void StartAudioTask (void *argument);
void StartLcdTask (void *argument);
void StartKeyboardTask (void *argument);
void StartLoggerTask (void *argument);
void StartSDTask (void *argument);

void MX_FREERTOS_Init (void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init (void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of LoggerMutex */
  LoggerMutexHandle = osMutexNew(&LoggerMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of LoggerBinarySem */
  LoggerBinarySemHandle = osSemaphoreNew(1, 1, &LoggerBinarySem_attributes);

  /* creation of LcdBinarySem */
  LcdBinarySemHandle = osSemaphoreNew(1, 1, &LcdBinarySem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of xLCDQueue */
  xLCDQueueHandle = osMessageQueueNew(16, sizeof(LCDTaskEvent_t), &xLCDQueue_attributes);

  /* creation of xAudioQueue */
//  xAudioQueueHandle = osMessageQueueNew (16, sizeof(AudioNotify_t), &xAudioQueue_attributes);
  /* creation of xUartQueue */
//  xUartQueueHandle = osMessageQueueNew (16, sizeof(UartEvent_t), &xUartQueue_attributes);
  /* creation of xLoggerQueue */
  xLoggerQueueHandle = osMessageQueueNew(16, sizeof(LogMessage_t), &xLoggerQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of UartTask */
  UartTaskHandle = osThreadNew(StartUartTask, NULL, &UartTask_attributes);

  /* creation of AudioTask */
  AudioTaskHandle = osThreadNew(StartAudioTask, NULL, &AudioTask_attributes);

  /* creation of LcdTask */
  LcdTaskHandle = osThreadNew(StartLcdTask, NULL, &LcdTask_attributes);

  /* creation of KeyboardTask */
  KeyboardTaskHandle = osThreadNew(StartKeyboardTask, NULL, &KeyboardTask_attributes);

  /* creation of LoggerTask */
  LoggerTaskHandle = osThreadNew(StartLoggerTask, NULL, &LoggerTask_attributes);

  /* creation of SDTask */
  SDTaskHandle = osThreadNew(StartSDTask, NULL, &SDTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of KeyboardEvent */
  KeyboardEventHandle = osEventFlagsNew(&KeyboardEvent_attributes);

  /* creation of SDEvent */
  SDEventHandle = osEventFlagsNew(&SDEvent_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask (void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  uint8_t count = 0;
  /* Infinite loop */
  for (;;)
  {
    LOG_DEBUG("DefaultTask %d", count++);
    osDelay(5000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartUartTask */
/**
 * @brief Function implementing the UartTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartUartTask */
void StartUartTask (void *argument)
{
  /* USER CODE BEGIN StartUartTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartUartTask */
}

/* USER CODE BEGIN Header_StartAudioTask */
/**
 * @brief Function implementing the AudioTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartAudioTask */
void StartAudioTask (void *argument)
{
  /* USER CODE BEGIN StartAudioTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartAudioTask */
}

/* USER CODE BEGIN Header_StartLcdTask */
/**
 * @brief Function implementing the LcdTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLcdTask */
void StartLcdTask (void *argument)
{
  /* USER CODE BEGIN StartLcdTask */
  hx8357_init();
  menu_init();

  LCDTaskEvent_t lcd_event;
  /* Infinite loop */
  for (;;)
  {
    if (xQueueReceive(xLCDQueueHandle, &lcd_event, portMAX_DELAY))
    {
      switch (lcd_event.event)
      {
	case LCD_EVENT_BTN:
	  menu_handle_button(lcd_event.btn);
	  break;
	case LCD_EVENT_PROGRESS:
	  update_progress_bar(lcd_event.value);
	  break;
	case LCD_EVENT_RTC:
	  update_date_time();
	  break;
	case LCD_EVENT_SCREEN:
	  change_screen(lcd_event.screen);
	  break;
      }
    }
  }
  /* USER CODE END StartLcdTask */
}

/* USER CODE BEGIN Header_StartKeyboardTask */
/**
 * @brief Function implementing the KeyboardTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartKeyboardTask */
void StartKeyboardTask (void *argument)
{
  /* USER CODE BEGIN StartKeyboardTask */
  /* Infinite loop */
  for (;;)
  {
//    printf("Processing keyboard task: waiting KEYBOARD_EVENT\r\n");
    osEventFlagsWait(KeyboardEventHandle, KEYBOARD_EVENT, osFlagsWaitAll, osWaitForever);
    osDelay(1);
  }
  /* USER CODE END StartKeyboardTask */
}

/* USER CODE BEGIN Header_StartLoggerTask */
/**
 * @brief Function implementing the LoggerTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLoggerTask */
void StartLoggerTask (void *argument)
{
  /* USER CODE BEGIN StartLoggerTask */
  logger_init();
  LogMessage_t msg;
  /* Infinite loop */
  for (;;)
  {
    if (osMessageQueueGet(xLoggerQueueHandle, &msg, NULL, osWaitForever) == osOK)
    {
      logger_process(&msg);
    }
  }
  /* USER CODE END StartLoggerTask */
}

/* USER CODE BEGIN Header_StartSDTask */
/**
 * @brief Function implementing the SDTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartSDTask */
void StartSDTask (void *argument)
{
  /* USER CODE BEGIN StartSDTask */
  if (sdfs_is_detected())
  {
    osEventFlagsSet(SDEventHandle, SD_EVENT);
  }
  /* Infinite loop */
  for (;;)
  {
    osEventFlagsWait(SDEventHandle, SD_EVENT, osFlagsWaitAll, osWaitForever);
    if (sdfs_is_detected())
    {
      printf("SD CONNECTED\r\n");
      sdfs_list_directory();
    }
    else
    {
      printf("SD DISCONNECTED\r\n");
    }
  }
  /* USER CODE END StartSDTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case KEYPAD_INT_Pin:
      osEventFlagsSet(KeyboardEventHandle, KEYBOARD_EVENT);
      break;
    case SD_DETECT_Pin:
      osEventFlagsSet(SDEventHandle, SD_EVENT);
      break;
  }
}
/* USER CODE END Application */

