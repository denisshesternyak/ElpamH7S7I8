#include "sdfs.h"
#include "defines.h"
#include "i2c.h"

static FRESULT res;
static DIR dir;
static FILINFO fno;
static WCHAR w_path[128];
static char full_path[128];
static int pos_path;
static uint8_t temp_mono_buf[AUDIO_HALF_BUFFER_SIZE] __attribute__((section(".extram")));
static BYTE buffer_copy[512] __attribute__((section(".extram")));
static WCHAR w_src_path[128];
static WCHAR w_dest_path[128];

typedef struct
{
  uint32_t magic_number;
  uint32_t total_size;
  uint32_t total_crc;
} FW_Header_t;

static bool sdfs_list_directory (const TCHAR *path,
				 char list[][FF_MAX_LFN],
				 uint8_t *size);

SDFS_STATE_t sdfs_state;

bool sdfs_is_detected (void)
{
  return HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_RESET;
}

bool sdfs_mount_drive (void)
{
  if(!sdfs_state.is_init)
    return false;

  if (sdfs_state.is_mounted)
    return true;

  bool res = f_mount(&SDFatFs, (TCHAR const*) ROOT_DIR_PATH_U, 1) == FR_OK;

  sdfs_state.is_mounted = res;
  return res;
}

void sdfs_unmount_drive (void)
{
  sdfs_state.is_mounted = f_mount(NULL, (TCHAR const*) ROOT_DIR_PATH_U, 1) == FR_OK;
}

static bool sdfs_list_directory (const TCHAR *path,
				 char list[][FF_MAX_LFN],
				 uint8_t *size)
{
  if (!sdfs_state.is_mounted)
  {
    sdfs_mount_drive();
    if (!sdfs_state.is_mounted)
    {
      return false;
    }
  }

  bool res;
  uint8_t count = 0;
  char ascii_name[FF_MAX_LFN + 1];

  res = f_opendir(&dir, path) == FR_OK;
  if (!res)
  {
    return false;
  }

  for (uint8_t item = 0; item < MAX_MENU_ITEMS; item++)
  {
    res = f_readdir(&dir, &fno);
    if (res != FR_OK || fno.fname[0] == 0)
      break;

    sdfs_convert_uni2eom(fno.fname, ascii_name);

    if (ascii_name[0] == '.' ||
	strcmp(ascii_name, "System Volume Information") == 0 ||
	fno.fattrib & AM_DIR)
      continue;

    if (list)
    {
      strncpy(list[item], ascii_name, FF_MAX_LFN);
      list[item][FF_MAX_LFN - 1] = '\0';
    }

    count++;
  }
  f_closedir(&dir);

  if (size)
    *size = count;

  return true;
}

void sdfs_list_alarms (char list[][FF_MAX_LFN], uint8_t *size)
{
  if (!sdfs_state.is_mounted)
    return;

  pos_path = snprintf(full_path, sizeof(full_path), "%s/", (char*) ALARMS_DIR_PATH);

  sdfs_list_directory(ALARMS_DIR_PATH_U, list, size);
}

void sdfs_list_messages (char list[][FF_MAX_LFN], uint8_t *size)
{
  if (!sdfs_state.is_mounted)
    return;

  pos_path = snprintf(full_path, sizeof(full_path), "%s/", (char*) MESSAGES_DIR_PATH);

  sdfs_list_directory(MESSAGES_DIR_PATH_U, list, size);
}

void sdfs_list_firmware (char list[][FF_MAX_LFN], uint8_t *size)
{
  if (!sdfs_state.is_mounted)
    return;

  pos_path = snprintf(full_path, sizeof(full_path), "%s/", (char*) FIRMWARE_DIR_PATH);

  sdfs_list_directory(FIRMWARE_DIR_PATH_U, list, size);
}

void sdfs_close_audiofile (AudioFileInfo_t *info)
{
  if (info->isOpened)
  {
    f_close(&SDFile);
    info->isOpened = false;
  }
}

static bool sdfs_read_wav_header (AudioFileInfo_t *info)
{
  res = f_read(&SDFile, &info->header, (UINT) AUDIO_HEADER_SIZE, (UINT*) &info->bytes_read);
  if (res != FR_OK || info->bytes_read != AUDIO_HEADER_SIZE)
  {
    f_close(&SDFile);
    return false;
  }

//    LOG_INFO("\r\n=== %s Header ===", (char *)info->filename);
//    LOG_INFO(" File size     : %lu bytes", (unsigned long)info->header.ChunkSize);
//    LOG_INFO(" Format        : %c%c%c%c", (char)info->header.Format, (char)(info->header.Format>>8), (char)(info->header.Format>>16), (char)(info->header.Format>>24));
//    LOG_INFO(" Sample Rate   : %lu Hz", (unsigned long)info->header.SampleRate);
//    LOG_INFO(" Channels      : %u", (unsigned)info->header.NumChannels);
//    LOG_INFO(" Bits/Sample   : %u", (unsigned)info->header.BitPerSample);
//    LOG_INFO(" ByteRate      : %lu", (unsigned long)info->header.ByteRate);
//    LOG_INFO(" Data ID       : %c%c%c%c", (char)info->header.SubChunk2ID, (char)(info->header.SubChunk2ID>>8), (char)(info->header.SubChunk2ID>>16), (char)(info->header.SubChunk2ID>>24));
//    LOG_INFO(" Data size     : %lu bytes", (unsigned long)info->header.SubChunk2Size);
//    LOG_INFO(" Data size hex : 0x%04lx bytes", info->header.SubChunk2Size);

//	uint32_t duration = 0;
//	if (info->header.ByteRate > 0)
//	{
//		duration = info->header.ChunkSize / info->header.ByteRate;
//		LOG_DEBUG(" Duration      : %02lu:%02lu", duration / 60, duration % 60);
//	}
  info->position = info->bytes_read;
  return true;
}

void sdfs_convert_eom2uni(char *src, WCHAR *dest)
{
  int j = 0;
  while (src[j] != 0 && j < FF_MAX_LFN)
  {
    dest[j] = ff_oem2uni(src[j], FF_CODE_PAGE);
    j++;
  }
  dest[j] = '\0';
}

void sdfs_convert_uni2eom(WCHAR *src, char *dest)
{
  int j = 0;
  while (src[j] != 0 && j < FF_MAX_LFN)
  {
    dest[j] = (char) ff_uni2oem(src[j], FF_CODE_PAGE);
    j++;
  }
  dest[j] = '\0';
}

bool sdfs_read_file_info (AudioFileInfo_t *info)
{
  snprintf(full_path + pos_path, sizeof(full_path) - pos_path, "%s", info->filename);

  sdfs_convert_eom2uni(full_path, w_path);

  res = f_open(&SDFile, (TCHAR*) w_path, FA_READ);
  if (res != FR_OK)
  {
    return false;
  }

  info->isOpened = true;
  info->isEnd = false;
  info->file_size = f_size(&SDFile);

  bool res = sdfs_read_wav_header(info);
  return res;
}

bool sdfs_read_file (AudioFileInfo_t *info,
		     uint8_t *buffer,
		     uint32_t buffer_len)
{
  uint32_t len = 0;
  if (!info->isOpened)
    return false;

  if (info->header.NumChannels == 2)
  {
    len = buffer_len;
    res = f_read(&SDFile, buffer, (UINT) len, (UINT*) &info->bytes_read);
  }
  else if (info->header.NumChannels == 1)
  {
    len = buffer_len >> 1;
    res = f_read(&SDFile, temp_mono_buf, (UINT) len, (UINT*) &info->bytes_read);

    if (res != FR_OK)
    {
      return false;
    }

    int16_t *src = (int16_t*) temp_mono_buf;
    int16_t *dst = (int16_t*) buffer;

    uint32_t sample_len = len >> 1;
    for (uint32_t i = 0; i < sample_len; i++)
    {
      int16_t sample = src[i];
      *(dst++) = sample;
      *(dst++) = sample;
    }
  }
  else
  {
    return false;
  }

  info->position += info->bytes_read;

  if (info->bytes_read < len || info->position >= info->file_size)
  {
    info->isEnd = true;
    memset(buffer + info->bytes_read, 0, len - info->bytes_read);
    info->position = info->file_size;
    return true;
  }

  return true;
}

static FRESULT sdfs_copy_file (const WCHAR *src_path, const WCHAR *dst_path)
{
    FIL src_file, dst_file;
    UINT bytes_read, bytes_written;
    FRESULT res;

    res = f_open(&src_file, src_path, FA_READ);
    if (res != FR_OK)
    {
        return res;
    }

    res = f_open(&dst_file, dst_path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK)
    {
        f_close(&src_file);
        return res;
    }

    while (1)
    {
        res = f_read(&src_file, buffer_copy, sizeof(buffer_copy), &bytes_read);
        if (res != FR_OK || bytes_read == 0)
            break;

        res = f_write(&dst_file, buffer_copy, bytes_read, &bytes_written);
        if (res != FR_OK || bytes_written != bytes_read)
        {
            res = FR_DISK_ERR;
            break;
        }
    }

    f_close(&src_file);
    f_close(&dst_file);

    return res;
}

bool sdfs_prepare_to_update (const char *src, const char *dest, char *path, size_t len)
{
  char temp_path[64];

  f_mkdir((const TCHAR*)BOOT_DIR_PATH_U);

  snprintf(temp_path, sizeof(temp_path), "%s/%s", FIRMWARE_DIR_PATH, src);
  sdfs_convert_eom2uni(temp_path, w_src_path);

  snprintf(path, len, "%s/%s", BOOT_DIR_PATH, dest);
  sdfs_convert_eom2uni(path, w_dest_path);

  res = sdfs_copy_file(w_src_path, w_dest_path);

  return res == FR_OK;
}


void sdfs_close_file(FIL* fp)
{
  f_close(fp);
}

bool sdfs_create_dir(const TCHAR *path)
{
  return f_mkdir(path) == FR_OK;
}

bool sdfs_is_file_exist(const TCHAR *path)
{
  FILINFO fno;
  return f_stat(path, &fno) == FR_OK;
}

bool sdfs_is_file_exist_not_null(const TCHAR *path)
{
  FILINFO fno;
  bool res = f_stat(path, &fno) == FR_OK;
  return res && fno.fsize > 0;
}

bool sdfs_send_to_backup(const TCHAR *path_old, const TCHAR *path_new)
{
  return f_rename(path_old, path_new) == FR_OK;
}

bool sdfs_open_from_last_entry (FIL* fp, const TCHAR* path)
{
  if (f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS | FA_READ) == FR_OK)
  {
    f_lseek(fp, f_size(fp));
    return true;
  }
  return false;
}

bool sdfs_open_file (FIL* fp, const TCHAR* path)
{
  return f_open(fp, path, FA_READ) == FR_OK;
}

void sdfs_write_data(FIL* fp, const void* data, uint32_t len)
{
  UINT bytes_written;
  f_write(fp, data, len, &bytes_written);
  f_sync(fp);
}

uint32_t sdfs_read_line (FIL *fp, char *buffer)
{
  if (fp == NULL || buffer == NULL)
    return 0;

  if (f_gets(w_dest_path, sizeof(w_dest_path), fp) == NULL)
    return 0;

  sdfs_convert_uni2eom(w_dest_path, buffer);

  uint32_t len = strlen(buffer);
  if (len >= 2 && buffer[len - 1] == '\n')
  {
    buffer[len - 1] = '\0';
    len--;
  }

  return len;
}

void sdfs_delete_directory(const TCHAR* path)
{
  DIR dir;
  FILINFO fno;

  if (f_opendir(&dir, path) == FR_OK)
  {
    while (f_readdir(&dir, &fno) == FR_OK)
    {
      if (fno.fname[0] == 0) break;
      if (fno.fattrib & AM_DIR) continue;

//      snprintf(full_path, sizeof(full_path), "%s/%s", path, fno.fname);
//      sdfs_convert_eom2uni(full_path, w_dest_path);
//
//      f_unlink((TCHAR*)w_dest_path);
    }
    f_closedir(&dir);
  }
  f_unlink(path);
}


