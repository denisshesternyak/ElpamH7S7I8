#ifndef INC_CODEC_AUDIO_H_
#define INC_CODEC_AUDIO_H_

#include "stdbool.h"
#include "audio_types.h"
#include "defines.h"
#include "stm32h7rsxx_hal.h"

#define ARMING_TIME  			10U
#define ANNOUNCEMENT_TIME  		180U
#define COUNT_PROGRESS  		10U

//#define CNVR_VOL(vol) 			((MIN_VOLUME_CODEC + ((vol) - MIN_VOLUME) * 55 / 42))
#define CNVR_VOL(vol) 			((MAX_VOLUME - (vol)) * 40 / 42)

void audio_init (void);
void audio_process (AudioNotify_t *audio_notify);
void audio_set_volume (uint8_t level);

void audio_notify_low (AudioEvent_t event, AudioType_t type);
void audio_notify_high (AudioEvent_t event, AudioType_t type);
void audio_notify_start_task_low (AudioType_t type, SinTask_t task, const char *name);

extern Audio_Player_t player;

#endif /* INC_CODEC_AUDIO_H_ */
