#ifndef INC_DTMF_DTMF_H_
#define INC_DTMF_DTMF_H_

#include "dtmf_types.h"
#include <stdbool.h>
#include <stdint.h>

void dtmf_init (void);
void dtmf_process (DTMFMessage_t *msg);

#endif /* INC_DTMF_DTMF_H_ */
