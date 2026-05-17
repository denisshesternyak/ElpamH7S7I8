#ifndef INC_FIRMWARE_FW_UPDATE_H_
#define INC_FIRMWARE_FW_UPDATE_H_

typedef enum
{
  FROM_A_TO_B,
  FROM_B_TO_A
} FW_slot_t;

void fw_process();

#endif /* INC_FIRMWARE_FW_UPDATE_H_ */
