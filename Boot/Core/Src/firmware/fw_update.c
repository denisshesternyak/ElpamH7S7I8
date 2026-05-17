#include "fw_update.h"
#include "ff.h"
#include "main.h"
#include "sd_diskio.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "extmem_manager.h"
#include "i2c.h"
#include "metadata.h"

typedef struct
{
  uint32_t magic_number;
  uint32_t total_size;
  uint32_t total_crc;
  uint16_t ver_major;
  uint16_t ver_minor;
} FW_Header_t;

#define FW_SALT 		"Elpam1234"
#define MAGIC_NUMBER 		0xDEADBEEF
#define SALT_SIZE 		(sizeof(FW_SALT) - 1)
#define MAGIC_SIZE 		sizeof(MAGIC_NUMBER)

#define FW_SLOT_OFFSET		0x01000000

#define EXT_FLASH_SECTOR_SIZE  	0x1000U

#define ROOT_DIR 		((TCHAR const*)"/")
#define FIRMWARE_DIR 		((TCHAR const*)"/boot")
#define FIRMWARE_BIN 		((TCHAR const*)"/boot/firmware.bin")

static bool fw_erase_flash (uint32_t sectrors, uint32_t addr);
static bool fw_write_flash (const uint8_t *data, uint32_t addr);
static bool fw_read_flash (uint8_t *data, uint32_t addr);
static bool fw_copy_flash (FW_slot_t slot);

static FRESULT res;
static FATFS SDFatFs;
static FIL SDFile;
char SDPath[4]; /* SD logical drive path */

static TCHAR fw_path[64];
static uint8_t flash_buffer[EXT_FLASH_SECTOR_SIZE];

static FW_Header_t header;
static uint32_t decrypt_offset = 0;

static void fw_update_xor_decrypt (uint8_t *data, uint16_t data_len)
{
  const char *salt = FW_SALT;
  uint8_t salt_len = SALT_SIZE;

  for (uint16_t i = 0; i < data_len; i++)
  {
    data[i] ^= salt[(decrypt_offset + i) % salt_len];
  }

  decrypt_offset += data_len;
}

static bool fw_erase_flash (uint32_t sectrors, uint32_t addr)
{
  return EXTMEM_EraseSector(EXTMEMORY_1, addr, sectrors * EXT_FLASH_SECTOR_SIZE) == EXTMEM_OK;
}

static bool fw_write_flash (const uint8_t *data, uint32_t addr)
{
  return EXTMEM_Write(EXTMEMORY_1, addr, data, EXT_FLASH_SECTOR_SIZE) == EXTMEM_OK;
}

static bool fw_read_flash (uint8_t *data, uint32_t addr)
{
  return EXTMEM_Read(EXTMEMORY_1, addr, data, EXT_FLASH_SECTOR_SIZE) == EXTMEM_OK;
}

//static void sdfs_list_directory ()
//{
//  DIR dir;
//  FILINFO fno;
//  res = f_opendir(&dir, FIRMWARE_DIR);
//  if (res != FR_OK)
//  {
//    return;
//  }
//
//  for (uint8_t item = 0; item < 10; item++)
//  {
//    res = f_readdir(&dir, &fno);
//    if (res != FR_OK || fno.fname[0] == 0)
//      break;
//
//    if (fno.fname[0] == '.' || strcmp(fno.fname, "System Volume Information") == 0)
//      continue;
//
//    if (fno.fattrib & AM_DIR)
//    {
//      printf(" [DIR]  %s\r\n", fno.fname);
//    }
//    else
//    {
//      printf(" [FILE] %s  (%lu bytes)\r\n", fno.fname, fno.fsize);
//    }
//  }
//  f_closedir(&dir);
//}

static bool fw_read_header ()
{
  uint16_t len = sizeof(FW_Header_t);
  uint16_t bytes_read = 0;
  decrypt_offset = 0;

  res = f_read(&SDFile, &header, (UINT) len, (UINT*) &bytes_read);
  if (res != FR_OK)
    return false;

  fw_update_xor_decrypt((uint8_t*) &header, len);

  if (header.magic_number != MAGIC_NUMBER)
    return false;

  decrypt_offset = sizeof(FW_Header_t);
  printf("MN: 0x%04lx, size: 0x%lx, ver %d.%d\r\n", header.magic_number, header.total_size, header.ver_major, header.ver_minor);

  return true;
}

static bool fw_update_fw (void)
{
  uint16_t bytes_read = 0;
  uint32_t total_bytes = 0;
  uint32_t addr_offset_write = 0;

  printf("Start writing to external flash...\r\n");

  uint32_t sectrors = (header.total_size / EXT_FLASH_SECTOR_SIZE) + 1;
  if (!fw_erase_flash(sectrors, addr_offset_write))
    return false;

  while (1)
  {
    res = f_read(&SDFile, flash_buffer, EXT_FLASH_SECTOR_SIZE, (UINT*) &bytes_read);
    if (res != FR_OK)
      return false;

    if (bytes_read == 0)
      break;

    fw_update_xor_decrypt(flash_buffer, bytes_read);

    if (bytes_read < EXT_FLASH_SECTOR_SIZE)
      memset(flash_buffer + bytes_read, 0xFF, EXT_FLASH_SECTOR_SIZE - bytes_read);

    if (!fw_write_flash(flash_buffer, addr_offset_write))
      return false;

    addr_offset_write += EXT_FLASH_SECTOR_SIZE;
    total_bytes += bytes_read;

    printf("Written: %lu / %lu bytes\r\n", total_bytes, header.total_size);
  }

  return true;
}

static bool fw_copy_flash (FW_slot_t slot)
{
  uint32_t total_bytes = 0;
  uint32_t addr_offset_read = 0;
  uint32_t addr_offset_write = 0;

  uint32_t total_size = 0;
  uint32_t total_crc = 0;

  switch (slot)
  {
    case FROM_A_TO_B:
      addr_offset_write = FW_SLOT_OFFSET;
      total_size = header.total_size;
      total_crc = header.total_crc;
      eeprom.total_size_slot_b = total_size;
      eeprom.crc_slot_b = total_crc;
      break;
    case FROM_B_TO_A:
      addr_offset_read = FW_SLOT_OFFSET;
      total_size = eeprom.total_size_slot_b;
      total_crc = eeprom.crc_slot_b;
      eeprom.total_size_slot_a = total_size;
      eeprom.crc_slot_a = total_crc;
      break;
  }

  uint32_t sectrors = (total_size / EXT_FLASH_SECTOR_SIZE) + 1;
  if (!fw_erase_flash(sectrors, addr_offset_write))
    return false;

  printf("Start copy to flash...\r\n");

  while (total_bytes < total_size)
  {
    uint32_t bytes_to_copy = EXT_FLASH_SECTOR_SIZE;
    if (total_bytes + bytes_to_copy > total_size)
      bytes_to_copy = total_size - total_bytes;

    if (!fw_read_flash(flash_buffer, addr_offset_read))
      return false;

    if (!fw_write_flash(flash_buffer, addr_offset_write))
      return false;

    addr_offset_read += EXT_FLASH_SECTOR_SIZE;
    addr_offset_write += EXT_FLASH_SECTOR_SIZE;
    total_bytes += bytes_to_copy;

    printf("Written: %lu / %lu bytes\r\n", total_bytes, total_size);
  }

  return true;
}

void fw_process ()
{
  if (!metadata_read())
  {
      snprintf(fw_path, sizeof(fw_path), "%s", FIRMWARE_BIN);
  }
  else
  {
      snprintf(fw_path, sizeof(fw_path), "%s/%.12s", FIRMWARE_DIR, eeprom.name);
  }

  fw_path[sizeof(fw_path) - 1] = '\0';

  printf("Firmware path: '%s'\r\n", fw_path);

  switch (eeprom.flag)
  {
    case FW_NO_UPDATES:
    case FW_SUCCESS_UPDATE:
      printf("No need for updating\r\n");
      return;
    case FW_UPDATED:
    case FW_BAD_UPDATE:
      if (!fw_copy_flash(FROM_B_TO_A))
      {
	eeprom.flag = (uint32_t) FW_NO_UPDATES;
	metadata_write();
	printf("FW rollback failed\r\n");
	return;
      }

      eeprom.flag = (uint32_t) FW_UPDATED;
      eeprom.len = 0;
      memset(eeprom.name, 0, sizeof(eeprom.name));
      metadata_write();

      printf("FW rollback success\r\n");
      return;
  }

  if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_RESET)
  {
    printf("SD not detected\r\n");
    return;
  }

  if (FATFS_LinkDriver(&SD_Driver, SDPath))
  {
    printf("Failed FATFS_LinkDriver\r\n");
    return;
  }

  if (f_mount(&SDFatFs, SDPath, 1) != FR_OK)
  {
    printf("Failed mounting\r\n");
    return;
  }

//  sdfs_list_directory();

  res = f_open(&SDFile, fw_path, FA_READ);
  if (res != FR_OK)
  {
    printf("Failed open\r\n");
    return;
  }

  if (!fw_read_header())
  {
    printf("Failed read header\r\n");
    f_close(&SDFile);
    return;
  }

  if (!fw_copy_flash(FROM_A_TO_B))
  {
    printf("FW reserve copy failed\r\n");
  }

  if (!fw_update_fw())
  {
    eeprom.flag = (uint32_t) FW_BAD_UPDATE;
    printf("FW update failure\r\n");
  }
  else
  {
    eeprom.flag = (uint32_t) FW_UPDATED;
    printf("FW updated success!\r\n");
  }

  eeprom.len = 0;
  memset(eeprom.name, 0, sizeof(eeprom.name));
  metadata_write();

  f_unlink(fw_path);

  f_close(&SDFile);
  f_mount(NULL, SDPath, 1);
}

