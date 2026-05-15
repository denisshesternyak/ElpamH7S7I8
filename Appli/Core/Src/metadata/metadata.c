#include "metadata.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>

#define EEPROM_HANDLER		&hi2c3
#define EEPROM_ADDR		0xA0
#define EEPROM_REG		0x00

FW_eeprom_t eeprom;

bool metadata_read()
{
  HAL_StatusTypeDef res;

  res = HAL_I2C_Mem_Read(EEPROM_HANDLER, EEPROM_ADDR, EEPROM_REG, I2C_MEMADD_SIZE_16BIT, (uint8_t *)&eeprom, sizeof(FW_eeprom_t), 100);
  if(res != HAL_OK)
      return false;

  return true;
}

bool metadata_write()
{
  HAL_StatusTypeDef res;

  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_RESET);
  res = HAL_I2C_Mem_Write(EEPROM_HANDLER, EEPROM_ADDR, EEPROM_REG, I2C_MEMADD_SIZE_16BIT, (uint8_t *)&eeprom, sizeof(FW_eeprom_t), 100);
  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_SET);

  while (res == HAL_OK && HAL_I2C_IsDeviceReady(EEPROM_HANDLER, EEPROM_ADDR, 3, 100) != HAL_OK);

  return true;
}

bool metadata_status(FW_status_t status)
{
  if(!metadata_read())
    return false;

  if(eeprom.flag == status)
    return false;

  eeprom.flag = (uint32_t)status;
  eeprom.len = 0;
  memset(eeprom.name, 0, sizeof(eeprom.name));

  if(!metadata_write())
    return false;

  return true;
}

bool metadata_update(const char *dest)
{
  if(!metadata_read())
    return false;

  eeprom.flag = (uint32_t)FW_READY_TO_UPDATE;
  eeprom.len = strlen(dest);
  memcpy(eeprom.name, dest, eeprom.len > 12 ? 12 : eeprom.len);

  if(!metadata_write())
    return false;

  return true;
}

