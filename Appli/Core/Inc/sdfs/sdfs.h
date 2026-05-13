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
}SDFS_STATE_t;

FRESULT sdfs_mount_drive (void);
void sdfs_unmount_drive (void);

void sdfs_list_alarms (char list[][FF_MAX_LFN], uint8_t *size);
void sdfs_list_messages (char list[][FF_MAX_LFN], uint8_t *size);
void sdfs_list_firmware (char list[][FF_MAX_LFN], uint8_t *size);

bool sdfs_read_file_info (AudioFileInfo_t *info);
bool sdfs_read_file (AudioFileInfo_t *info,
		     uint8_t *buffer,
		     uint32_t buffer_len);
void sdfs_close_file (AudioFileInfo_t *info);

bool sdfs_is_detected (void);

extern SDFS_STATE_t sdfs_state;

#endif /* INC_SDFS_SDFS_H_ */
