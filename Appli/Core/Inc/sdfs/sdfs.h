#ifndef INC_SDFS_SDFS_H_
#define INC_SDFS_SDFS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "fatfs.h"
#include "audio_types.h"

typedef struct
{
  bool is_mounted;
  bool is_init;
} SDFS_STATE_t;

bool sdfs_mount_drive (void);
void sdfs_unmount_drive (void);

void sdfs_list_alarms (char list[][FF_MAX_LFN], uint8_t *size);
void sdfs_list_messages (char list[][FF_MAX_LFN], uint8_t *size);
void sdfs_list_firmware (char list[][FF_MAX_LFN], uint8_t *size);

bool sdfs_read_file_info (AudioFileInfo_t *info);
bool sdfs_read_file (AudioFileInfo_t *info, uint8_t *buffer, uint32_t buffer_len);
void sdfs_close_audiofile (AudioFileInfo_t *info);
void sdfs_close_file (FIL *fp);
bool sdfs_is_detected (void);
bool sdfs_create_dir (const TCHAR *dir);
bool sdfs_prepare_to_update (const char *src, const char *dest, char *path, size_t len);
bool sdfs_is_file_exist (const TCHAR *path);
bool sdfs_is_file_exist_not_null (const TCHAR *path);
bool sdfs_send_to_backup (const TCHAR *path_old, const TCHAR *path_new);
bool sdfs_open_from_last_entry (FIL *fp, const TCHAR *path);
bool sdfs_open_file (FIL* fp, const TCHAR* path);
void sdfs_write_data (FIL *fp, const void *data, uint32_t len);
uint32_t sdfs_read_line (FIL *fp, TCHAR *buffer, uint32_t buf_size);
void sdfs_delete_directory (const TCHAR *path);

void sdfs_convert_eom2uni (char *src, WCHAR *dest);

extern SDFS_STATE_t sdfs_state;

#endif /* INC_SDFS_SDFS_H_ */
