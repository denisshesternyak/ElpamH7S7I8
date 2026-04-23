#ifndef INC_CODEC_AUDIO_GENERATE_SIN_H_
#define INC_CODEC_AUDIO_GENERATE_SIN_H_

#include "stdint.h"
#include "audio_types.h"

#define WAVE_TABLE_SIZE 	1024
#define SAMPLE_RATE     	44100
#define FADE_IN_SAMPLE     	8820	// 200ms * 44100 / 1000
#define FADE_IN_STEP     	3805	// 32767 / 8820 * 1024

#define FREQ_836 		836
#define FREQ_856		856

//(1U << 32) / (SAMPLE_RATE / FREQ)
#define PHASE_INC_100		9739154U
#define PHASE_INC_250		24347887U
#define PHASE_INC_300		29217464U
#define PHASE_INC_420		40904450U
#define PHASE_INC_425		41391408U
#define PHASE_INC_500		48695774U
#define PHASE_INC_836		81419334U
#define PHASE_INC_856		83367165U
#define PHASE_INC_1000		97391548U
#define PHASE_INC_1020		99339379U
#define PHASE_INC_20000		1947830973U

typedef struct
{
  uint32_t intervals[3];
  uint8_t interval_count;
  uint8_t current_step;
  uint32_t step_start_tick;
} interval_timer_t;

typedef struct
{
  uint64_t samples_per_phase[3];
  uint64_t ramp_counter;
  uint64_t total_samples_generated;
  uint64_t total_samples_fade;
  uint64_t duration;
  uint32_t target_phase[3];
  uint32_t current_inc;
  uint32_t target_inc;
  uint32_t add_inc[2];
  uint32_t stable_freq_phase;
  uint8_t current_step;
  bool stable_middle;
  bool fade_stop;
} inc_data_t;

void audio_init_sin_table (void);
void audio_generate_sine (Audio_Player_t *player, uint8_t *buffer, uint32_t samples_per_channel);
void init_generation (SinTask_t sinus);
void set_increment (uint32_t value_ch1, uint32_t value_ch2);
void set_fade_stop ();

#endif /* INC_CODEC_AUDIO_GENERATE_SIN_H_ */
