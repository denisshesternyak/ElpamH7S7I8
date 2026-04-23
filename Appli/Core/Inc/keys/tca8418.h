#ifndef INC_KEYS_TCA8418_H_
#define INC_KEYS_TCA8418_H_

#include <stdbool.h>
#include <stdint.h>
#include <tca8418_reg.h>
#include "stm32h7rsxx_hal.h"

#define TCA8418_I2C_ADDR        (0x34 << 1)

typedef enum
{
  TCA8418_R0 = 0,
  TCA8418_R1,
  TCA8418_R2,
  TCA8418_R3,
  TCA8418_R4,
  TCA8418_R5,
  TCA8418_R6,
  TCA8418_R7,
  TCA8418_C0 = 8,
  TCA8418_C1,
  TCA8418_C2,
  TCA8418_C3,
  TCA8418_C4,
  TCA8418_C5,
  TCA8418_C6,
  TCA8418_C7,
  TCA8418_C8 = 16,
  TCA8418_C9
} TCA8418_Pin_t;

HAL_StatusTypeDef TCA8418_Init (I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef TCA8418_SetKeypadPin (I2C_HandleTypeDef *hi2c,
					TCA8418_Pin_t pin,
					bool enableKeypad);
HAL_StatusTypeDef TCA8418_SetEventFIFO (I2C_HandleTypeDef *hi2c,
					TCA8418_Pin_t pin,
					bool enableFIFO);
HAL_StatusTypeDef TCA8418_EnableKeyInterrupt (I2C_HandleTypeDef *hi2c,
					      bool enable);
HAL_StatusTypeDef TCA8418_ClearInterrupt (I2C_HandleTypeDef *hi2c);

uint8_t TCA8418_GetEventCount (I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef TCA8418_ReadKeyEvent (I2C_HandleTypeDef *hi2c,
					uint8_t *code,
					bool *pressed);

#endif /* INC_KEYS_TCA8418_H_ */

