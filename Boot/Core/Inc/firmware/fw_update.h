#ifndef INC_FW_UPDATE_H_
#define INC_FW_UPDATE_H_

#define CMD_READY           0x01
#define CMD_START_UPDATE    0x10
#define CMD_DATA_PACKET     0x11
#define CMD_END_UPDATE      0x12
#define CMD_STATISTIC       0x13
#define CMD_ACK             0xFF
#define CMD_NACK            0xEE

#define MAX_PACKET_DATA     256
#define RX_BUFFER_SIZE      (MAX_PACKET_DATA + 20)
#define MAX_ERROR_COUNT     5

#define EXT_FLASH_SECTOR_SIZE  0x1000U

typedef enum
{
  FW_OK,
  FW_NSOF,
  FW_NSIZE,
  FW_NRECEIVE,
  FW_NEOF,
  FW_NCRC
}FW_STATUS;

void fw_update_process (void);

#endif /* INC_FW_UPDATE_H_ */
