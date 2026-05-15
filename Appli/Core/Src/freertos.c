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
#include "fatfs.h"
#include "usart.h"
#include "adc.h"
#include "app_freertos.h"
#include "events.h"
#include "sdfs.h"
#include "logger.h"
#include "lcd_menu.h"
#include "rs232.h"
#include "command_dispatcher.h"
#include "system_status.h"
#include "audio.h"
#include "keyboard.h"
#include "analog.h"
#include "tests.h"
#include "sdmmc.h"
#include "metadata.h"

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
//#define SELFTEST
#ifdef SELFTEST
__IO bool is_selftest = false;
osThreadId_t TestsTaskHandle;
const osThreadAttr_t testsTask_attributes = {
    .name = "testsTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal1, };

void StartTestsTask (void *argument);
#endif
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
void HAL_RTCEx_WakeUpTimerEventCallback (RTC_HandleTypeDef *hrtc)
{
#ifdef SELFTEST
  if (is_selftest)
    return;
#endif

  static LCDTaskEvent_t lcd_event = { .event = LCD_EVENT_RTC };
  osMessageQueuePut(xLCDQueueHandle, &lcd_event, 0, 0);

  static AudioNotify_t audio_notify = {
      .event = AUDIO_TIMER,
      .type = AUDIO_NONE,
      .priority = AUDIO_PRIORITY_LOW };
  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 0);

}
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
  xAudioQueueHandle = osMessageQueueNew(16, sizeof(AudioNotify_t), &xAudioQueue_attributes);

  /* creation of xUartQueue */
  xUartQueueHandle = osMessageQueueNew(16, sizeof(UartEvent_t), &xUartQueue_attributes);

  /* creation of xLoggerQueue */
  xLoggerQueueHandle = osMessageQueueNew(16, sizeof(LogMessage_t), &xLoggerQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
#ifdef SELFTEST
  TestsTaskHandle = osThreadNew(StartTestsTask, NULL, &testsTask_attributes);
#endif
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
  analog_init();
//  osDelay(1000);
//  LOG_DEBUG("12v %d, 24v %d, cur %d, amp %d, drv %d",
//		  getRData(1, get_adc_value(ADC_12V)),
//		  getRData(1, get_adc_value(ADC_24V)),
//		  getRData(1, get_adc_value(ADC_CURRENT_MEAS)),
//		  getRData(1, get_adc_value(ADC_AMPLIFIER_MEAS)),
//		  getRData(1, get_adc_value(ADC_DRIVER_MEAS)));

  uint8_t flag = 0;
  const uint32_t last_time = osKernelGetTickCount();
  /* Infinite loop */
  for (;;)
  {
    if (!flag && (osKernelGetTickCount() - last_time) > FW_SUCCESS_TIMEOUT)
    {
      metadata_status(FW_SUCCESS_UPDATE);
      flag = 1;
    }

    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
//    LOG_DEBUG("DefaultTask %d", count++);
    osDelay(500);
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
  rs232_init();

  // === Register all handlers from  command_dispatcher ===
  rs232_register_arm(handle_arm);
  rs232_register_all_clear_1(handle_all_clear_1);
  rs232_register_all_clear_2(handle_all_clear_2);
  rs232_register_alarm(handle_alarm);
  rs232_register_chemical(handle_chemical);
  rs232_register_disarm(handle_disarm);
  rs232_register_cancel(handle_cancel);
  rs232_register_quiet_test(handle_quiet_test);
  rs232_register_reserve_1(handle_reserve_1);
  rs232_register_reserve_2(handle_reserve_2);
  rs232_register_reserve_3(handle_reserve_3);
  rs232_register_remote_pa(handle_remote_pa);
  rs232_register_reset(handle_reset);
  rs232_register_volume_up(volume_up_handler);
  rs232_register_volume_down(volume_down_handler);
  rs232_register_report(system_fill_report);
  rs232_register_unknown(handle_unknown_command);

  rs232_register_enter(handle_enter_command);
  rs232_register_up(handle_up_command);
  rs232_register_down(handle_down_command);
  rs232_register_esc(handle_esc_command);
  rs232_register_cnlbtn(handle_cancel_command);
  rs232_register_test(handle_test_command);
  rs232_register_announc(handle_announc_command);
  rs232_register_message(handle_message_command);
  rs232_register_almbtn(handle_alarm_command);
  rs232_register_armbtn(handle_arm_command);

  rs232_register_amp_t1(handle_amp_t1_command);
  rs232_register_amp_t2(handle_amp_t2_command);
  rs232_register_amp_t3(handle_amp_t3_command);
  rs232_register_amp_t4(handle_amp_t4_command);
  rs232_register_amp_t5(handle_amp_t5_command);
  rs232_register_amp_t6(handle_amp_t6_command);
  rs232_register_amp_t7(handle_amp_t7_command);
  rs232_register_amp_t8(handle_amp_t8_command);
  rs232_register_amp_t9(handle_amp_t9_command);
  rs232_register_amp_t10(handle_amp_t10_command);
  rs232_register_amp_on(handle_amp_on_command);
  rs232_register_drv_on(handle_drv_on_command);
  rs232_register_amp_st(handle_amp_st_command);

  system_status_init();
  UartEvent_t event;
  /* Infinite loop */
  for (;;)
  {
    if (osMessageQueueGet(xUartQueueHandle, &event, NULL, osWaitForever) == osOK)
    {
      rs232_process(event);
    }
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
  audio_init();

  AudioNotify_t audio_notify;

  /* Infinite loop */
  for (;;)
  {
    if (osMessageQueueGet(xAudioQueueHandle, &audio_notify, NULL, osWaitForever) == osOK)
    {
      audio_process(&audio_notify);
    }
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
#ifndef SELFTEST
  hx8357_init();
#endif
  menu_init();

//  osDelay(1000);
//  audio_notify_start_task_low(AUDIO_SIN, SINUS_1000HZ_120S, NULL);
//  osDelay(5000);
//  audio_notify_low(AUDIO_PREPARE_STOP, AUDIO_SIN);

  LCDTaskEvent_t lcd_event;
  /* Infinite loop */
  for (;;)
  {
    if (osMessageQueueGet(xLCDQueueHandle, &lcd_event, NULL, osWaitForever) == osOK)
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
  keyboard_init();
  KeyEvent_t ev;
  /* Infinite loop */
  for (;;)
  {
    osEventFlagsWait(KeyboardEventHandle, KEYBOARD_EVENT, osFlagsWaitAll, osWaitForever);
    keyboard_process(&ev);

    if(ev.pressed)
    {
      LCDTaskEvent_t lcd_event = {.event = LCD_EVENT_BTN, .btn = ev};
      osMessageQueuePut(xLCDQueueHandle, &lcd_event, 0, 100);
    }

    ev.pressed = false;
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
    osDelay(50);

    if (sdfs_is_detected())
    {
      LOG_INFO("uSD CONNECTED");

      if (!sdfs_state.is_init)
      {
	MX_SDMMC1_SD_Init();
	sdfs_state.is_init = true;
      }

      if (!sdfs_state.is_mounted)
      {
	sdfs_mount_drive();
//	sdfs_list_messages(NULL, NULL);
      }
    }
    else
    {
      LOG_INFO("uSD DISCONNECTED");
      if (sdfs_state.is_mounted)
      {
	sdfs_unmount_drive();
      }

      if (sdfs_state.is_init)
      {
	HAL_SD_DeInit(&hsd1);
	sdfs_state.is_init = false;
      }

      FATFS_UnLinkDriver(SDPath);
    }
  }
  /* USER CODE END StartSDTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
#ifdef SELFTEST
void StartTestsTask (void *argument)
{
  vTaskSuspendAll();

  is_selftest = true;

  osThreadTerminate(UartTaskHandle);
  osThreadTerminate(AudioTaskHandle);
  osThreadTerminate(LcdTaskHandle);
  osThreadTerminate(KeyboardTaskHandle);
  osThreadTerminate(LoggerTaskHandle);
  osThreadTerminate(SDTaskHandle);

  xTaskResumeAll();

  HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_SET);

  hx8357_init();

  test_board();

  UartTaskHandle = osThreadNew(StartUartTask, NULL, &UartTask_attributes);
  AudioTaskHandle = osThreadNew(StartAudioTask, NULL, &AudioTask_attributes);
  LcdTaskHandle = osThreadNew(StartLcdTask, NULL, &LcdTask_attributes);
  KeyboardTaskHandle = osThreadNew(StartKeyboardTask, NULL, &KeyboardTask_attributes);
  LoggerTaskHandle = osThreadNew(StartLoggerTask, NULL, &LoggerTask_attributes);
  SDTaskHandle = osThreadNew(StartSDTask, NULL, &SDTask_attributes);

  is_selftest = false;
  osThreadTerminate(TestsTaskHandle);

  /* Infinite loop */
  for (;;)
  {
  }
}
#endif
/* USER CODE END Application */

