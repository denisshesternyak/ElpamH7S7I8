#include "metadata.h"
#include "i2c.h"
#include "crc.h"
#include <string.h>
#include <stdio.h>
#include "logger.h"
#include "cmsis_os2.h"
#include "app_freertos.h"

#define EEPROM_HANDLER       (&hi2c3)
#define EEPROM_ADDR          0xA0
#define EEPROM_REG           0x00
#define EEPROM_I2C_TIMEOUT   100
#define EEPROM_WRITE_RETRY   3
#define EEPROM_READY_RETRY   3

#define EEPROM_MAX_OBJECT_SIZE 64

FW_eeprom_t eeprom;

static bool eeprom_read_raw (uint16_t mem_addr, void *out, uint32_t size)
{
  HAL_StatusTypeDef res = HAL_I2C_Mem_Read(
  EEPROM_HANDLER,
  EEPROM_ADDR, mem_addr,
  I2C_MEMADD_SIZE_16BIT, (uint8_t*) out, size,
  EEPROM_I2C_TIMEOUT);

  return (res == HAL_OK);
}

static bool eeprom_write_raw (uint16_t mem_addr, const void *in, uint32_t size)
{
  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_RESET);

  HAL_StatusTypeDef res = HAL_ERROR;

  for (int retry = 0; retry < EEPROM_WRITE_RETRY; retry++)
  {
    res = HAL_I2C_Mem_Write(
    EEPROM_HANDLER,
    EEPROM_ADDR, mem_addr,
    I2C_MEMADD_SIZE_16BIT, (uint8_t*) in, size,
    EEPROM_I2C_TIMEOUT);

    if (res == HAL_OK)
      break;

    for (int i = 0; i < 1000; i++)
      __NOP();
  }

  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_SET);

  if (res != HAL_OK)
    return false;

  uint32_t start = HAL_GetTick();

  while (HAL_I2C_IsDeviceReady(
  EEPROM_HANDLER,
  EEPROM_ADDR,
  EEPROM_READY_RETRY,
  EEPROM_I2C_TIMEOUT) != HAL_OK)
  {
    if (HAL_GetTick() - start > EEPROM_I2C_TIMEOUT)
      return false;
  }

  return true;
}

static uint32_t crc32_of (const void *data, uint32_t size)
{
  __HAL_CRC_DR_RESET(&hcrc);

  uint32_t words = (size + 3U) / 4U;

  uint32_t buf[words];

  memset(buf, 0, sizeof(buf));
  memcpy(buf, data, size);

  return HAL_CRC_Calculate(&hcrc, buf, words);
}

static bool eeprom_read_object_locked (uint16_t mem_addr,
				       void *out,
				       uint32_t size)
{
  uint32_t stored_crc = 0;

  if (!eeprom_read_raw(mem_addr, out, size))
    return false;

  if (!eeprom_read_raw(mem_addr + size, &stored_crc, sizeof(stored_crc)))
    return false;

  uint32_t calc_crc = crc32_of(out, size);

  if (calc_crc != stored_crc)
  {
    LOG_ERROR("EEPROM[0x%04X] CRC error! Expected 0x%08lX, got 0x%08lX\r\n", mem_addr, (unsigned long )stored_crc, (unsigned long )calc_crc);
    return false;
  }
  return true;
}

static bool eeprom_write_object_locked (uint16_t mem_addr,
					const void *in,
					uint32_t size)
{
  uint32_t crc = crc32_of(in, size);

  uint8_t buf[sizeof(uint32_t) + EEPROM_MAX_OBJECT_SIZE];

  if (size + sizeof(crc) > sizeof(buf))
    return false;

  memcpy(buf, in, size);
  memcpy(buf + size, &crc, sizeof(crc));

  return eeprom_write_raw(mem_addr, buf, size + sizeof(crc));
}

bool eeprom_read_object (uint16_t mem_addr, void *out, uint32_t size)
{
  if (out == NULL || size == 0)
    return false;

  if (osMutexAcquire(MetaMutexHandle, osWaitForever) != osOK)
    return false;

  bool result = eeprom_read_object_locked(mem_addr, out, size);

  osMutexRelease(MetaMutexHandle);

  return result;
}

bool eeprom_write_object (uint16_t mem_addr, const void *in, uint32_t size)
{
  if (in == NULL || size == 0)
    return false;

  if (osMutexAcquire(MetaMutexHandle, osWaitForever) != osOK)
    return false;

  bool result = eeprom_write_object_locked(mem_addr, in, size);

  osMutexRelease(MetaMutexHandle);

  return result;
}

bool eeprom_update_object (uint16_t mem_addr,
			   void *obj,
			   uint32_t size,
			   void (*on_corrupt) (void *obj))
{
  if (obj == NULL || size == 0)
    return false;

  if (osMutexAcquire(MetaMutexHandle, osWaitForever) != osOK)
    return false;

  bool result = true;

  if (!eeprom_read_object_locked(mem_addr, obj, size))
  {
    if (on_corrupt)
      on_corrupt(obj);
  }

  result = eeprom_write_object_locked(mem_addr, obj, size);

  osMutexRelease(MetaMutexHandle);

  return result;
}

static uint32_t fw_crc (const FW_eeprom_t *d)
{
  return crc32_of(d, offsetof(FW_eeprom_t, crc32));
}

static void fw_defaults (FW_eeprom_t *d)
{
  memset(d, 0, sizeof(*d));

  d->magic = EEPROM_MAGIC;
  d->flag = FW_OK;
}

static bool metadata_read_locked (void)
{
  FW_eeprom_t temp;

  if (!eeprom_read_raw(EEPROM_SLOT_METADATA, &temp, sizeof(temp)))
    return false;

  if (temp.magic != EEPROM_MAGIC)
  {
    LOG_WARN("EEPROM metadata: no magic\r\n");
    return false;
  }

  uint32_t saved_crc = temp.crc32;
  uint32_t calc_crc = fw_crc(&temp);

  if (calc_crc != saved_crc)
  {
    LOG_ERROR("EEPROM metadata CRC error! Expected 0x%08lX, got 0x%08lX\r\n", (unsigned long )saved_crc, (unsigned long )calc_crc);
    return false;
  }

  memcpy(&eeprom, &temp, sizeof(eeprom));
  return true;
}

static bool metadata_write_locked (void)
{
  FW_eeprom_t temp;

  memcpy(&temp, &eeprom, sizeof(temp));

  temp.magic = EEPROM_MAGIC;
  temp.crc32 = fw_crc(&temp);

  if (!eeprom_write_raw(
  EEPROM_SLOT_METADATA, &temp, sizeof(temp)))
  {
    LOG_ERROR("EEPROM metadata write failed\r\n");
    return false;
  }

  memcpy(&eeprom, &temp, sizeof(eeprom));
  return true;
}

static bool metadata_init_defaults_locked (void)
{
  fw_defaults(&eeprom);
  return metadata_write_locked();
}

bool metadata_read (void)
{
  if (osMutexAcquire(MetaMutexHandle, osWaitForever) != osOK)
    return false;

  bool result = metadata_read_locked();
  osMutexRelease(MetaMutexHandle);
  return result;
}

bool metadata_write (void)
{
  if (osMutexAcquire(MetaMutexHandle, osWaitForever) != osOK)
    return false;

  bool result = metadata_write_locked();
  osMutexRelease(MetaMutexHandle);
  return result;
}

bool metadata_status (FW_status_t status)
{
  if (osMutexAcquire(MetaMutexHandle, osWaitForever) != osOK)
    return false;

  bool result = false;

  if (eeprom.flag != (uint32_t) status)
  {
    eeprom.flag = (uint32_t) status;
    result = true;
  }

  osMutexRelease(MetaMutexHandle);
  return result;
}

bool metadata_status_update (FW_status_t status)
{
  if (osMutexAcquire(MetaMutexHandle, osWaitForever) != osOK)
    return false;

  bool result = false;

  if (!metadata_read_locked())
  {
    result = metadata_init_defaults_locked();
  }
  else if (eeprom.flag != (uint32_t) status)
  {
    eeprom.flag = (uint32_t) status;
    result = metadata_write_locked();
  }

  osMutexRelease(MetaMutexHandle);
  return result;
}

bool metadata_path_update (const char *dest, uint32_t len)
{
  if (osMutexAcquire(MetaMutexHandle, osWaitForever) != osOK)
    return false;

  bool result = false;

  if (metadata_read_locked())
  {
    eeprom.flag = (uint32_t) FW_UPDATE_READY;
    eeprom.len = len;

    snprintf(eeprom.path, sizeof(eeprom.path), "%s", dest ? dest : "");
    result = metadata_write_locked();
  }

  osMutexRelease(MetaMutexHandle);
  return result;
}
