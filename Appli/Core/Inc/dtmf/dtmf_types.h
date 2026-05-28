#ifndef INC_DTMF_DTMF_TYPES_H_
#define INC_DTMF_DTMF_TYPES_H_

#include <stdint.h>

#define DTMF_BUFFER_RX_SIZE  			512
#define DTMF_HALF_BUFFER_RX_SIZE  		256

#define DTMF_COUNT_NUM  			8
#define DTMF_HALF_COUNT_NUM  			(DTMF_COUNT_NUM >> 1)

typedef enum
{
  DTMF_IDLE,
  DTMF_START,
  DTMF_CHECK,
  DTMF_STOP
} DTMFEvent_t;

typedef struct
{
  DTMFEvent_t event;
  uint8_t *data;
} DTMFMessage_t;

#endif /* INC_DTMF_DTMF_TYPES_H_ */
