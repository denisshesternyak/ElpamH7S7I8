#include "metadata.h"
#include "i2c.h"
#include "crc.h"
#include <string.h>
#include <stdio.h>

#define EEPROM_HANDLER      (&hi2c3)
#define EEPROM_ADDR         0xA0
#define EEPROM_REG          0x00
#define EEPROM_I2C_TIMEOUT  100
#define EEPROM_WRITE_RETRY  3
#define EEPROM_READY_RETRY  3

FW_eeprom_t eeprom;

static bool eeprom_read_raw (uint16_t mem_addr, void *out, uint32_t size)
{
  HAL_StatusTypeDef res = HAL_I2C_Mem_Read(
      EEPROM_HANDLER, EEPROM_ADDR, mem_addr,
      I2C_MEMADD_SIZE_16BIT, (uint8_t *)out, size, EEPROM_I2C_TIMEOUT);
  return res == HAL_OK;
}

static bool eeprom_write_raw (uint16_t mem_addr, const void *in, uint32_t size)
{
  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_RESET);

  HAL_StatusTypeDef res = HAL_ERROR;
  for (int retry = 0; retry < EEPROM_WRITE_RETRY; retry++)
  {
    res = HAL_I2C_Mem_Write(EEPROM_HANDLER, EEPROM_ADDR, mem_addr,
                            I2C_MEMADD_SIZE_16BIT,
                            (uint8_t *)in, size, EEPROM_I2C_TIMEOUT);
    if (res == HAL_OK)
      break;
    for (int i = 0; i < 1000; i++) __NOP();
  }

  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_SET);

  if (res == HAL_OK)
  {
    while (HAL_I2C_IsDeviceReady(EEPROM_HANDLER, EEPROM_ADDR,
                                 EEPROM_READY_RETRY, EEPROM_I2C_TIMEOUT) != HAL_OK);
  }
  return res == HAL_OK;
}

static uint32_t crc32_of(const void *data, uint32_t size)
{
    __HAL_CRC_DR_RESET(&hcrc);

    uint32_t words = (size + 3u) / 4u;

    uint32_t buf[words];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, data, size);

    return HAL_CRC_Calculate(&hcrc, buf, words);
}

bool eeprom_read_object (uint16_t mem_addr, void *out, uint32_t size)
{
  uint32_t stored_crc = 0;

  if (!eeprom_read_raw(mem_addr, out, size))
    return false;

  if (!eeprom_read_raw(mem_addr + size, &stored_crc, sizeof(stored_crc)))
    return false;

  uint32_t calc_crc = crc32_of(out, size);
  if (calc_crc != stored_crc)
  {
    return false;
  }
  return true;
}

bool eeprom_write_object(uint16_t mem_addr, const void *in, uint32_t size)
{
  uint32_t crc = crc32_of(in, size);
  uint8_t  buf[sizeof(uint32_t) + 64];

  if (size + sizeof(crc) > sizeof(buf))
    return false;

  memcpy(buf, in, size);
  memcpy(buf + size, &crc, sizeof(crc));

  return eeprom_write_raw(mem_addr, buf, size + sizeof(crc));
}

bool eeprom_update_object (uint16_t mem_addr, void *obj, uint32_t size,
                          void (*on_corrupt)(void *obj))
{
  if (!eeprom_read_object(mem_addr, obj, size))
  {
    if (on_corrupt)
      on_corrupt(obj);
  }

  if (!eeprom_write_object(mem_addr, obj, size))
    return false;

  return true;
}

static uint32_t fw_crc(const FW_eeprom_t *d)
{
  return crc32_of(d, offsetof(FW_eeprom_t, crc32));
}

static void fw_defaults(FW_eeprom_t *d)
{
  memset(d, 0, sizeof(*d));
  d->magic = EEPROM_MAGIC;
  d->flag  = FW_OK;
}

static bool metadata_init_defaults(void)
{
  fw_defaults(&eeprom);
  return metadata_write();
}

bool metadata_read (void)
{
  FW_eeprom_t temp;

  if (!eeprom_read_raw(EEPROM_SLOT_METADATA, &temp, sizeof(temp)))
    return false;

  if (temp.magic != EEPROM_MAGIC)
  {
    return false;
  }

  uint32_t saved_crc = temp.crc32;
  uint32_t calc_crc = fw_crc(&temp);

  if (calc_crc != saved_crc)
  {
      return false;
  }

  memcpy(&eeprom, &temp, sizeof(eeprom));
  return true;
}

bool metadata_write(void)
{
  FW_eeprom_t temp;

  memcpy(&temp, &eeprom, sizeof(temp));

  temp.magic = EEPROM_MAGIC;
  temp.crc32 = fw_crc(&temp);

  if (!eeprom_write_raw(
	  EEPROM_SLOT_METADATA,
	  &temp,
	  sizeof(temp)))
  {
      return false;
  }

  memcpy(&eeprom, &temp, sizeof(eeprom));

  return true;
}

bool metadata_status (FW_status_t status)
{
  if (eeprom.flag == (uint32_t)status)
    return false;

  eeprom.flag = (uint32_t)status;

  return true;
}

bool metadata_status_update (FW_status_t status)
{
  if (!metadata_read())
    return metadata_init_defaults();

  if (eeprom.flag == (uint32_t)status)
    return false;

  eeprom.flag = (uint32_t)status;

  return metadata_write();
}

bool metadata_path_update (const char *dest, uint32_t len)
{
  if (!metadata_read())
    return false;

  eeprom.flag = (uint32_t)FW_UPDATE_READY;
  eeprom.len  = len;
  snprintf(eeprom.path, sizeof(eeprom.path), "%s", dest ? dest : "");

  return metadata_write();
}
