/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    extmemloader_init.h
  * @author  MCD Application Team
  * @brief   Header file of Loader_Src.c
  *
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EXTMEMLOADER_INIT_H
#define EXTMEMLOADER_INIT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

uint32_t extmemloader_Init(void);
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#define CODEC_RESET_Pin GPIO_PIN_13
#define CODEC_RESET_GPIO_Port GPIOC
#define CODEC_CS_Pin GPIO_PIN_4
#define CODEC_CS_GPIO_Port GPIOE
#define LCD_PWM_Pin GPIO_PIN_9
#define LCD_PWM_GPIO_Port GPIOE
#define CODEC_RESETB15_Pin GPIO_PIN_15
#define CODEC_RESETB15_GPIO_Port GPIOB
#define KEYPAD_RST_Pin GPIO_PIN_7
#define KEYPAD_RST_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOA
#define SD_DETECT_Pin GPIO_PIN_1
#define SD_DETECT_GPIO_Port GPIOG
#define CODEC_INT_Pin GPIO_PIN_7
#define CODEC_INT_GPIO_Port GPIOD
#define KEYPAD_INT_Pin GPIO_PIN_2
#define KEYPAD_INT_GPIO_Port GPIOG
#define LCD_DC_Pin GPIO_PIN_1
#define LCD_DC_GPIO_Port GPIOE
#endif /* EXTMEMLOADER_INIT_H */
