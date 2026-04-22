#include "sdfs.h"
#include "fatfs.h"
#include "logger.h"

#define ROOT_DIR_PATH 		((TCHAR*)u"/")
#define ALARMS_DIR_PATH 	((TCHAR*)u"/alarms")
#define MESSAGES_DIR_PATH 	((TCHAR*)u"/messages")

void sdfs_list_directory (void)
{
  FRESULT res;
  DIR dir;
  FILINFO fno;

  res = f_mount(&SDFatFs, (TCHAR const*) SDPath, 0);
  if (res != FR_OK)
  {
    LOG_ERROR("f_mount failed with code: %d\r\n", res);
    return;
  }

  res = f_opendir(&dir, MESSAGES_DIR_PATH);
  if (res != FR_OK)
  {
    LOG_ERROR("f_opendir failed: %d\r\n", res);
  }

  char ascii_name[FF_MAX_LFN + 1];

  LOG_DEBUG("SD card: \r\n");

  for (;;)
  {
    res = f_readdir(&dir, &fno);
    if (res != FR_OK || fno.fname[0] == 0)
      break;

    int j = 0;
    while (fno.fname[j] != 0 && j < FF_MAX_LFN)
    {
      char ch = (char) ff_uni2oem(fno.fname[j], FF_CODE_PAGE);
      ascii_name[j] = ch;
      j++;
    }
    ascii_name[j] = '\0';

    if (ascii_name[0] == '.' || strcmp(ascii_name, "System Volume Information") == 0)
      continue;

    if (fno.fattrib & AM_DIR)
      LOG_DEBUG(" [DIR]  %s\r\n", ascii_name);
    else
      LOG_DEBUG(" [FILE] %s  (%lu bytes)\r\n", ascii_name, fno.fsize);
  }
  f_closedir(&dir);
}

bool sdfs_is_detected (void)
{
  return HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_RESET;
}
