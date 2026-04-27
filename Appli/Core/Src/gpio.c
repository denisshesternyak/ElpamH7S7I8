/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "app_freertos.h"
#include "events.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOO_CLK_ENABLE();
  __HAL_RCC_GPION_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOM_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DISPLAY_RST_Pin|KEYPAD_RST_Pin|RS485_DE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CODEC_CS_Pin|LCD_PWM_Pin|AMP_T1_ACT_Pin|AMP_T2_ACT_Pin
                          |AMP_T3_ACT_Pin|LCD_DC_Pin|AMP_T10_ACT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, AMP_T8_ACT_Pin|AMP_T9_ACT_Pin|AMP_ON_ACT_Pin|LED_3_Pin
                          |AMP_T4_ACT_Pin|AMP_T5_ACT_Pin|AMP_T6_ACT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BAT_OFF_ACT_GPIO_Port, BAT_OFF_ACT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_1_Pin|CODEC_RESET_Pin|LED_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CU_RESET_ACT_GPIO_Port, CU_RESET_ACT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(AMP_T7_ACT_GPIO_Port, AMP_T7_ACT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DRV_ON_ACT_GPIO_Port, DRV_ON_ACT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BAT_MTS_ACT_GPIO_Port, BAT_MTS_ACT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DISPLAY_RST_Pin KEYPAD_RST_Pin */
  GPIO_InitStruct.Pin = DISPLAY_RST_Pin|KEYPAD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : POW_DET_Pin SOL_DET_Pin OVER_VO_Pin UNDER_VO_Pin
                           PPF_PD0_DOOR_Pin */
  GPIO_InitStruct.Pin = POW_DET_Pin|SOL_DET_Pin|OVER_VO_Pin|UNDER_VO_Pin
                          |PPF_PD0_DOOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CODEC_CS_Pin LCD_DC_Pin */
  GPIO_InitStruct.Pin = CODEC_CS_Pin|LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : AMP_T8_ACT_Pin AMP_T9_ACT_Pin AMP_ON_ACT_Pin LED_3_Pin
                           AMP_T4_ACT_Pin AMP_T5_ACT_Pin AMP_T6_ACT_Pin */
  GPIO_InitStruct.Pin = AMP_T8_ACT_Pin|AMP_T9_ACT_Pin|AMP_ON_ACT_Pin|LED_3_Pin
                          |AMP_T4_ACT_Pin|AMP_T5_ACT_Pin|AMP_T6_ACT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PPF_PD1_FLOOD_Pin */
  GPIO_InitStruct.Pin = PPF_PD1_FLOOD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PPF_PD1_FLOOD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BAT_OFF_ACT_Pin */
  GPIO_InitStruct.Pin = BAT_OFF_ACT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BAT_OFF_ACT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_1_Pin CODEC_RESET_Pin LED_2_Pin */
  GPIO_InitStruct.Pin = LED_1_Pin|CODEC_RESET_Pin|LED_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_PWM_Pin AMP_T1_ACT_Pin AMP_T2_ACT_Pin AMP_T3_ACT_Pin
                           AMP_T10_ACT_Pin */
  GPIO_InitStruct.Pin = LCD_PWM_Pin|AMP_T1_ACT_Pin|AMP_T2_ACT_Pin|AMP_T3_ACT_Pin
                          |AMP_T10_ACT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : EP_WP_Pin */
  GPIO_InitStruct.Pin = EP_WP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EP_WP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CU_RESET_ACT_Pin */
  GPIO_InitStruct.Pin = CU_RESET_ACT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CU_RESET_ACT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_DE_Pin */
  GPIO_InitStruct.Pin = RS485_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RS485_DE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : AMP_T7_ACT_Pin */
  GPIO_InitStruct.Pin = AMP_T7_ACT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(AMP_T7_ACT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DRV_ON_ACT_Pin */
  GPIO_InitStruct.Pin = DRV_ON_ACT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DRV_ON_ACT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_DETECT_Pin */
  GPIO_InitStruct.Pin = SD_DETECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SD_DETECT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BAT_MTS_ACT_Pin */
  GPIO_InitStruct.Pin = BAT_MTS_ACT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BAT_MTS_ACT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CODEC_INT_Pin */
  GPIO_InitStruct.Pin = CODEC_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CODEC_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : KEYPAD_INT_Pin */
  GPIO_InitStruct.Pin = KEYPAD_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(KEYPAD_INT_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
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
/* USER CODE END 2 */
