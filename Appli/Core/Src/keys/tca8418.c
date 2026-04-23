#include "tca8418.h"
#include "logger.h"

static HAL_StatusTypeDef TCA8418_WriteReg (I2C_HandleTypeDef *hi2c,
					   uint8_t reg,
					   uint8_t value)
{
  return HAL_I2C_Mem_Write(hi2c, TCA8418_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}

static HAL_StatusTypeDef TCA8418_ReadReg (I2C_HandleTypeDef *hi2c,
					  uint8_t reg,
					  uint8_t *value)
{
  return HAL_I2C_Mem_Read(hi2c, TCA8418_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, value, 1, 100);
}

HAL_StatusTypeDef TCA8418_Init (I2C_HandleTypeDef *hi2c)
{
  HAL_StatusTypeDef ret;

  ret = TCA8418_WriteReg(hi2c, TCA8418_REG_CFG, TCA8418_CFG_AI | TCA8418_CFG_KE_IEN);
  if (ret != HAL_OK)
    return ret;

  ret = TCA8418_WriteReg(hi2c, TCA8418_REG_INT_STAT, TCA8418_CFG_KE_IEN);
  if (ret != HAL_OK)
    return ret;

  return HAL_OK;
}

HAL_StatusTypeDef TCA8418_SetKeypadPin (I2C_HandleTypeDef *hi2c,
					TCA8418_Pin_t pin,
					bool enableKeypad)
{
  uint8_t reg = TCA8418_REG_KP_GPIO_1 + (pin / 8);
  uint8_t bit = pin % 8;

  uint8_t val;
  if (TCA8418_ReadReg(hi2c, reg, &val) != HAL_OK)
    return HAL_ERROR;

  if (enableKeypad)
    val |= (1 << bit);
  else
    val &= ~(1 << bit);

  return TCA8418_WriteReg(hi2c, reg, val);
}

HAL_StatusTypeDef TCA8418_SetEventFIFO (I2C_HandleTypeDef *hi2c,
					TCA8418_Pin_t pin,
					bool enableFIFO)
{
  uint8_t reg = TCA8418_REG_GPI_EM_1 + (pin / 8);
  uint8_t bit = pin % 8;

  uint8_t val;
  if (TCA8418_ReadReg(hi2c, reg, &val) != HAL_OK)
    return HAL_ERROR;

  if (enableFIFO)
    val |= (1 << bit);
  else
    val &= ~(1 << bit);

  return TCA8418_WriteReg(hi2c, reg, val);
}

HAL_StatusTypeDef TCA8418_EnableKeyInterrupt (I2C_HandleTypeDef *hi2c,
					      bool enable)
{
  uint8_t cfg;
  if (TCA8418_ReadReg(hi2c, TCA8418_REG_CFG, &cfg) != HAL_OK)
    return HAL_ERROR;

  if (enable)
    cfg |= TCA8418_CFG_KE_IEN;
  else
    cfg &= ~TCA8418_CFG_KE_IEN;

  return TCA8418_WriteReg(hi2c, TCA8418_REG_CFG, cfg);
}

HAL_StatusTypeDef TCA8418_ClearInterrupt (I2C_HandleTypeDef *hi2c)
{
  return TCA8418_WriteReg(hi2c, TCA8418_REG_INT_STAT, TCA8418_INT_K_INT);
}

uint8_t TCA8418_GetEventCount (I2C_HandleTypeDef *hi2c)
{
  uint8_t reg;
  if (TCA8418_ReadReg(hi2c, TCA8418_REG_KEY_LCK_EC, &reg) != HAL_OK)
    return 0;
  return (reg & 0x0F);
}

HAL_StatusTypeDef TCA8418_ReadKeyEvent (I2C_HandleTypeDef *hi2c,
					uint8_t *code,
					bool *pressed)
{
  uint8_t raw;
  if (TCA8418_ReadReg(hi2c, TCA8418_REG_KEY_EVENT_A, &raw) != HAL_OK)
    return HAL_ERROR;

  uint8_t keycode = raw & 0x7F;
  if (keycode == 0)
    return HAL_ERROR;

  *code = keycode;
  *pressed = (raw & 0x80) != 0;

//  event->row = keycode / 10;
//  event->col = keycode % 10;

  return HAL_OK;
}
