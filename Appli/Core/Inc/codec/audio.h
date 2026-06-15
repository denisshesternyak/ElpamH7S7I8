#ifndef INC_CODEC_AUDIO_H_
#define INC_CODEC_AUDIO_H_

#include "stdbool.h"
#include "audio_types.h"
#include "defines.h"
#include "stm32h7rsxx_hal.h"

#define COUNT_PROGRESS  		10U

//#define CNVR_VOL(vol) 			((MIN_VOLUME_CODEC + ((vol) - MIN_VOLUME) * 55 / 42))
#define CNVR_VOL(vol) 			((MAX_VOLUME - (vol)) * 40 / 42)

void audio_init (void);
void audio_process (AudioNotify_t *audio_notify);
void audio_set_volume (uint8_t level);
void audio_set_volume_playback (uint8_t level);
void audio_get_volume_level(uint8_t *level, uint8_t *value);
void audio_arm_timeout (void);
void audio_announcement_timeout (void);
void audio_recording_timeout (void);

void audio_notify_low (AudioEvent_t event, AudioType_t type);
void audio_notify_medium (AudioEvent_t event, AudioType_t type);
void audio_notify_high (AudioEvent_t event, AudioType_t type);
void audio_notify_start_task_low (AudioType_t type, SinTask_t task, const char *name);
void audio_notify_arming (bool val);

bool audio_is_playing ();
bool audio_is_stoped ();
bool audio_is_recording ();
bool audio_is_announcement ();
bool audio_is_arming ();
bool audio_is_motorola ();

AudioType_t audio_get_type ();

extern Audio_Player_t player;

#endif /* INC_CODEC_AUDIO_H_ */
