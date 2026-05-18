#ifndef INC_METADATA_METADATA_H_
#define INC_METADATA_METADATA_H_

#include <stdbool.h>
#include <stdint.h>

#define EEPROM_MAGIC 		0xEE0100EE

typedef struct
{
  uint32_t magic;
  uint32_t crc32;
  uint32_t flag;
  uint32_t total_size_slot_a;
  uint32_t total_size_slot_b;
  uint32_t crc_slot_a;
  uint32_t crc_slot_b;
  uint32_t len;
  char path[32];
} FW_eeprom_t;

typedef enum
{
  FW_OK,
  FW_UPDATE_SUCCESS,
  FW_UPDATE_READY,
  FW_ROLLBACK
} FW_status_t;

#define UPDATE_FILE    		"firmware.bin"
#define FW_SUCCESS_TIMEOUT	5000

bool metadata_read (void);
bool metadata_write (void);
bool metadata_status (FW_status_t status);
bool metadata_status_update (FW_status_t status);
bool metadata_path_update (const char *dest, uint32_t len);

extern FW_eeprom_t eeprom;

#endif /* INC_METADATA_METADATA_H_ */
