/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file   fatfs.c
 * @brief  Code for fatfs applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define ROOT_DIR_PATH 		((TCHAR*)u"/")
#define ALARMS_DIR_PATH 	((TCHAR*)u"/alarms")
#define MESSAGES_DIR_PATH 	((TCHAR*)u"/messages")
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static uint32_t PinDetect =
  { SD_DETECT_Pin };
static GPIO_TypeDef *SD_GPIO_PORT =
  { SD_DETECT_GPIO_Port };

uint32_t osQueueMsg;

osThreadId_t FSAppThreadHandle;
const osThreadAttr_t uSDThread_attributes =
  { .name = "uSDThread", .priority = (osPriority_t) osPriorityNormal,
      .stack_size = 512 * 8 };

/* Definitions for Mutex */
osMessageQueueId_t QueueHandle;
const osMessageQueueAttr_t Queue_attributes =
  { .name = "osqueue" };

FATFS SDFatFs; /* File system object for SD logical drive */
FIL SDFile; /* File  object for SD */
char SDPath[4]; /* SD logical drive path */
const MKFS_PARM OptParm =
  { FM_ANY, 0, 0, 0, 0 };

static uint32_t CARD_CONNECTED = 0;
static uint32_t CARD_DISCONNECTED = 1;
static uint32_t CARD_STATUS_CHANGED = 2;

//static uint8_t isFsCreated = 1;
static __IO uint8_t statusChanged = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void uSDThread_Entry (void *argument);
static void FS_FileOperations (void);
static uint8_t SD_IsDetected (void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

void MX_FATFS_Init(void)
{
  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /*## FatFS: Link the disk I/O driver(s)  ###########################*/
  if (FATFS_LinkDriver(&SD_DMA_Driver, SDPath) == 0)
  {
    /* creation of uSDThread */
    FSAppThreadHandle = osThreadNew(uSDThread_Entry, NULL,
				    &uSDThread_attributes);

    /* Create Storage Message Queue */
    QueueHandle = osMessageQueueNew(1U, sizeof(uint16_t), NULL);
  }
  /* USER CODE END Init */
}

/* USER CODE BEGIN Application */
/**
 * @brief  Start task
 * @param  pvParameters not used
 * @retval None
 */
static void uSDThread_Entry (void *argument)
{
  osStatus_t status;

  if (SD_IsDetected())
  {
    osMessageQueuePut(QueueHandle, &CARD_CONNECTED, 100, 0U);
  }

  osDelay(200);

  /* Infinite Loop */
  for (;;)
  {
    status = osMessageQueueGet(QueueHandle, &osQueueMsg, NULL, 100);

    if ((status == osOK) && (osQueueMsg == CARD_STATUS_CHANGED))
    {
      if (SD_IsDetected())
      {
	osMessageQueuePut(QueueHandle, &CARD_CONNECTED, 100, 0U);
      }
      else
      {
	osMessageQueuePut(QueueHandle, &CARD_DISCONNECTED, 100, 0U);
      }
    }

    if ((status == osOK) && (osQueueMsg == CARD_CONNECTED))
    {
      printf("Card CONNECTED\r\n");
      FS_FileOperations();
      statusChanged = 0;
    }

    if ((status == osOK) && (osQueueMsg == CARD_DISCONNECTED))
    {
      printf("Card DISCONNECTED\r\n");
      osDelay(200);

      f_mount(NULL, (TCHAR const*) "", 0);
      statusChanged = 0;
    }
  }
}

/**
 * @brief File system : file operation
 * @retval File operation result
 */
static void FS_FileOperations (void)
{
  FRESULT res;
  DIR dir;
  FILINFO fno;

  res = f_mount(&SDFatFs, (TCHAR const*) SDPath, 0);
  if (res != FR_OK)
  {
    printf("f_mount failed with code: %d\r\n", res);
    return;
  }

  /* check whether the FS has been already created */
//    if (isFsCreated == 0)
//    {
//      if (f_mkfs(SDPath, &OptParm, workBuffer, sizeof(workBuffer)) != FR_OK)
//      {
//	printf("Creates an FAT\r\n");
//	return;
//      }
//      isFsCreated = 1;
//    }
  res = f_opendir(&dir, MESSAGES_DIR_PATH);
  if (res != FR_OK)
  {
    printf("f_opendir failed: %d\r\n", res);
  }

  char ascii_name[FF_MAX_LFN+1];

  printf("SD card: \r\n");

  for (;;)
  {
    res = f_readdir(&dir, &fno);
    if (res != FR_OK || fno.fname[0] == 0)
      break;

    int j = 0;
    while (fno.fname[j] != 0 && j < FF_MAX_LFN)
    {
      char ch = (char) ff_uni2oem(fno.fname[j], FF_CODE_PAGE);
      ascii_name[j] = ch;
      j++;
    }
    ascii_name[j] = '\0';

    if (ascii_name[0] == '.'
	|| strcmp(ascii_name, "System Volume Information") == 0)
      continue;

    if (fno.fattrib & AM_DIR)
      printf(" [DIR]  %s\r\n", ascii_name);
    else
      printf(" [FILE] %s  (%lu bytes)\r\n", ascii_name, fno.fsize);
  }
  f_closedir(&dir);

}

static uint8_t SD_IsDetected (void)
{
  uint8_t status;

  if (HAL_GPIO_ReadPin(SD_GPIO_PORT, PinDetect) == GPIO_PIN_RESET)
  {
    status = HAL_ERROR;
  }
  else
  {
    status = HAL_OK;
  }
  return status;
}

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None.
 */

//void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
//{
//  if (GPIO_Pin == SD_DETECT_Pin)
//  {
//    if (statusChanged == 0)
//    {
//      statusChanged = 1;
//      osMessageQueuePut(QueueHandle, &CARD_STATUS_CHANGED, 100, 0U);
//    }
//  }
//}
/* USER CODE END Application */
