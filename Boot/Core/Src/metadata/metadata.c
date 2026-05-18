#include "metadata.h"
#include "i2c.h"
#include "crc.h"
#include <string.h>
#include <stdio.h>

#define EEPROM_HANDLER		&hi2c3
#define EEPROM_ADDR		0xA0
#define EEPROM_REG		0x00

FW_eeprom_t eeprom;

static uint32_t metadata_calc_crc (FW_eeprom_t *data)
{
  uint32_t saved_crc = data->crc32;
  data->crc32 = 0;

  uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t*) data, sizeof(FW_eeprom_t) / 4);

  data->crc32 = saved_crc;
  return crc;
}

bool metadata_read ()
{
  FW_eeprom_t temp;
  HAL_StatusTypeDef res;

  res = HAL_I2C_Mem_Read(EEPROM_HANDLER, EEPROM_ADDR, EEPROM_REG,
  I2C_MEMADD_SIZE_16BIT, (uint8_t*) &temp, sizeof(FW_eeprom_t), 100);
  if (res != HAL_OK)
    return false;

  if (temp.magic != EEPROM_MAGIC)
  {
    memset(&eeprom, 0, sizeof(FW_eeprom_t));
    eeprom.magic = EEPROM_MAGIC;
    eeprom.flag = FW_OK;
    metadata_write();
    return true;
  }

  uint32_t calc_crc = metadata_calc_crc(&temp);
  if (calc_crc != temp.crc32)
  {
    printf("CRC error! Expected: 0x%08lX, Got: 0x%08lX\r\n", temp.crc32, calc_crc);
    return false;
  }

  memcpy(&eeprom, &temp, sizeof(FW_eeprom_t));
  return true;
}

bool metadata_write()
{
  HAL_StatusTypeDef res;
  FW_eeprom_t temp;

  memcpy(&temp, &eeprom, sizeof(FW_eeprom_t));
  temp.crc32 = metadata_calc_crc(&temp);
  temp.magic = EEPROM_MAGIC;

  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_RESET);

  for (int retry = 0; retry < 3; retry++)
  {
    res = HAL_I2C_Mem_Write(EEPROM_HANDLER, EEPROM_ADDR, EEPROM_REG,
                            I2C_MEMADD_SIZE_16BIT, (uint8_t*)&temp, sizeof(FW_eeprom_t), 100);
    if (res == HAL_OK)
      break;

    for(int i=0; i<1000; i++)
    {
      __NOP();
    }
  }

  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_SET);

  while (res == HAL_OK && HAL_I2C_IsDeviceReady(EEPROM_HANDLER, EEPROM_ADDR, 3, 100) != HAL_OK);

  FW_eeprom_t verify;
  res = HAL_I2C_Mem_Read(EEPROM_HANDLER, EEPROM_ADDR, EEPROM_REG,
                         I2C_MEMADD_SIZE_16BIT, (uint8_t*)&verify, sizeof(FW_eeprom_t), 100);

  if (res == HAL_OK && verify.crc32 == temp.crc32 && verify.magic == temp.magic)
    return true;

  return false;
}

bool metadata_status (FW_status_t status)
{
  if (eeprom.flag == status)
    return false;

  eeprom.flag = (uint32_t) status;

  return true;
}

bool metadata_status_update (FW_status_t status)
{
  if (!metadata_read())
    return false;

  if (eeprom.flag == status)
    return false;

  eeprom.flag = (uint32_t) status;

  if (!metadata_write())
    return false;

  return true;
}

bool metadata_path_update (const char *dest, uint32_t len)
{
  if (!metadata_read())
    return false;

  eeprom.flag = (uint32_t) FW_UPDATE_READY;
  eeprom.len = len;
  snprintf(eeprom.path, sizeof(eeprom.path), "%s", dest);

  if (!metadata_write())
    return false;

  return true;
}

