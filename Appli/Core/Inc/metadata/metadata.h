#ifndef INC_METADATA_METADATA_H_
#define INC_METADATA_METADATA_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  uint32_t flag;
  uint32_t version;
  uint32_t len;
  uint8_t name[12];
} FW_eeprom_t;

typedef enum
{
  FW_NOT_UPDATE,
  FW_UPDATED,
  FW_SUCCESS_UPDATE,
  FW_READY_TO_UPDATE,
  FW_BAD_UPDATE
}FW_status_t;

#define UPDATE_FILE    		"firmware.bin"
#define FW_SUCCESS_TIMEOUT	5000

bool metadata_read();
bool metadata_write();
bool metadata_status(FW_status_t status);
bool metadata_update(const char *dest);

extern FW_eeprom_t eeprom;

#endif /* INC_METADATA_METADATA_H_ */
