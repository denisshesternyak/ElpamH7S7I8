/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

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
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DISPLAY_RST_Pin GPIO_PIN_13
#define DISPLAY_RST_GPIO_Port GPIOC
#define POW_DET_Pin GPIO_PIN_3
#define POW_DET_GPIO_Port GPIOE
#define CODEC_CS_Pin GPIO_PIN_4
#define CODEC_CS_GPIO_Port GPIOE
#define AMP_T8_ACT_Pin GPIO_PIN_5
#define AMP_T8_ACT_GPIO_Port GPIOF
#define AMP_T9_ACT_Pin GPIO_PIN_8
#define AMP_T9_ACT_GPIO_Port GPIOF
#define PPF_PD1_FLOOD_Pin GPIO_PIN_9
#define PPF_PD1_FLOOD_GPIO_Port GPIOF
#define AMP_ON_ACT_Pin GPIO_PIN_10
#define AMP_ON_ACT_GPIO_Port GPIOF
#define BAT_OFF_ACT_Pin GPIO_PIN_1
#define BAT_OFF_ACT_GPIO_Port GPIOH
#define LED_1_Pin GPIO_PIN_2
#define LED_1_GPIO_Port GPIOB
#define SOL_DET_Pin GPIO_PIN_7
#define SOL_DET_GPIO_Port GPIOE
#define LCD_PWM_Pin GPIO_PIN_9
#define LCD_PWM_GPIO_Port GPIOE
#define OVER_VO_Pin GPIO_PIN_10
#define OVER_VO_GPIO_Port GPIOE
#define EP_WP_Pin GPIO_PIN_1
#define EP_WP_GPIO_Port GPIOO
#define CODEC_RESET_Pin GPIO_PIN_15
#define CODEC_RESET_GPIO_Port GPIOB
#define CU_RESET_ACT_Pin GPIO_PIN_7
#define CU_RESET_ACT_GPIO_Port GPION
#define KEYPAD_RST_Pin GPIO_PIN_7
#define KEYPAD_RST_GPIO_Port GPIOC
#define RS485_DE_Pin GPIO_PIN_8
#define RS485_DE_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOA
#define UNDER_VO_Pin GPIO_PIN_11
#define UNDER_VO_GPIO_Port GPIOE
#define AMP_T1_ACT_Pin GPIO_PIN_13
#define AMP_T1_ACT_GPIO_Port GPIOE
#define AMP_T2_ACT_Pin GPIO_PIN_14
#define AMP_T2_ACT_GPIO_Port GPIOE
#define AMP_T3_ACT_Pin GPIO_PIN_15
#define AMP_T3_ACT_GPIO_Port GPIOE
#define AMP_T7_ACT_Pin GPIO_PIN_3
#define AMP_T7_ACT_GPIO_Port GPIOD
#define DRV_ON_ACT_Pin GPIO_PIN_0
#define DRV_ON_ACT_GPIO_Port GPIOG
#define SD_DETECT_Pin GPIO_PIN_1
#define SD_DETECT_GPIO_Port GPIOG
#define BAT_MTS_ACT_Pin GPIO_PIN_13
#define BAT_MTS_ACT_GPIO_Port GPIOM
#define CODEC_INT_Pin GPIO_PIN_7
#define CODEC_INT_GPIO_Port GPIOD
#define KEYPAD_INT_Pin GPIO_PIN_2
#define KEYPAD_INT_GPIO_Port GPIOG
#define LED_3_Pin GPIO_PIN_0
#define LED_3_GPIO_Port GPIOF
#define AMP_T4_ACT_Pin GPIO_PIN_1
#define AMP_T4_ACT_GPIO_Port GPIOF
#define AMP_T5_ACT_Pin GPIO_PIN_2
#define AMP_T5_ACT_GPIO_Port GPIOF
#define AMP_T6_ACT_Pin GPIO_PIN_3
#define AMP_T6_ACT_GPIO_Port GPIOF
#define LED_2_Pin GPIO_PIN_4
#define LED_2_GPIO_Port GPIOB
#define PPF_PD0_DOOR_Pin GPIO_PIN_0
#define PPF_PD0_DOOR_GPIO_Port GPIOE
#define LCD_DC_Pin GPIO_PIN_1
#define LCD_DC_GPIO_Port GPIOE
#define AMP_T10_ACT_Pin GPIO_PIN_2
#define AMP_T10_ACT_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
