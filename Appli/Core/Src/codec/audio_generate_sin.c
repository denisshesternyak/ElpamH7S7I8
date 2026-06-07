#include "audio_generate_sin.h"
#include "string.h"
#include <math.h>

static uint32_t phase_inc_ch1;
static uint32_t phase_inc_ch2;

static int16_t sin_table[WAVE_TABLE_SIZE];
static uint32_t phase_acc_ch1 = 0;
static uint32_t phase_acc_ch2 = 0;

//interval_timer_t interval_timer;
inc_data_t inc_data;

static void init_constant_tone (uint32_t freq_left_inc,
				uint32_t freq_right_inc,
				uint32_t duration_ms);
static void init_cycle_tone (uint32_t start_inc,
			     uint32_t end_inc,
			     uint32_t mid_inc,
			     uint32_t ending_ms,
			     uint32_t mid_ms);

static void apply_fade (int16_t *left, int16_t *right, uint64_t sample_idx);
static void apply_fade_out (int16_t *left, int16_t *right, uint64_t remaining);
static void generate_sample (uint8_t *buffer,
			     uint32_t idx,
			     int16_t left,
			     int16_t right);
static void update_phase_and_increment (void);
static bool handle_ramp_transition (uint8_t *buffer,
				    Audio_Player_t *player,
				    uint32_t samples_per_channel,
				    uint32_t current_sample_in_buffer);

void audio_init_sin_table (void)
{
  for (int i = 0; i < WAVE_TABLE_SIZE; i++)
  {
    sin_table[i] = (int16_t) (sinf(M_TWOPI * i / WAVE_TABLE_SIZE) * 32767);
  }
}

void audio_generate_sine (Audio_Player_t *player,
			  uint8_t *buffer,
			  uint32_t samples_per_channel)
{
  for (uint32_t i = 0; i < samples_per_channel; i++)
  {
    uint32_t buffer_idx = i * 4;

    uint32_t index_ch1 = (phase_acc_ch1 >> 22) & (WAVE_TABLE_SIZE - 1);
    uint32_t index_ch2 = (phase_acc_ch2 >> 22) & (WAVE_TABLE_SIZE - 1);

    int16_t sample_left = sin_table[index_ch1];
    int16_t sample_right = sin_table[index_ch2];

    inc_data.total_samples_generated++;
    inc_data.ramp_counter++;

    if (inc_data.total_samples_generated < FADE_IN_SAMPLE)
    {
      apply_fade(&sample_left, &sample_right, inc_data.total_samples_generated);
    }
    else if (inc_data.fade_stop)
    {
      uint64_t fade_out = inc_data.total_samples_fade - inc_data.total_samples_generated;
      apply_fade_out(&sample_left, &sample_right, fade_out);

      if (fade_out == 0)
      {
	uint32_t bytes_to_clear = (samples_per_channel - i - 1) * 4;
	if (bytes_to_clear > 0)
	  memset(buffer + buffer_idx + 4, 0, bytes_to_clear);

	player->duration = 100;
	player->is_prepare_stoped = true;
	player->is_fade_stoped = true;
	return;
      }
    }
    else if (inc_data.total_samples_generated > inc_data.duration - FADE_IN_SAMPLE)
    {
      uint64_t fade_out = inc_data.duration - inc_data.total_samples_generated;
      apply_fade_out(&sample_left, &sample_right, fade_out);
    }

    generate_sample(buffer, buffer_idx, sample_left, sample_right);

    update_phase_and_increment();

    if (handle_ramp_transition(buffer, player, samples_per_channel, i))
    {
      return;
    }
  }

  player->duration = (inc_data.total_samples_generated * 100ULL) / inc_data.duration;
}

void init_generation (SinTask_t sinus)
{
//	memset(&interval_timer, 0, sizeof(interval_timer));
//	interval_timer.current_step = 0;
//	interval_timer.step_start_tick = osKernelGetTickCount();

  switch (sinus)
  {
    case SINUS_420HZ_120S:
      init_constant_tone(PHASE_INC_420, PHASE_INC_420, 120000);
      break;
    case SINUS_1000HZ_120S:
      init_constant_tone(PHASE_INC_1000, PHASE_INC_1000, 120000);
      break;
    case SINUS_1020HZ_120S:
      init_constant_tone(PHASE_INC_1020, PHASE_INC_1020, 120000);
      break;
    case SINUS_20000HZ_120S:
      init_constant_tone(PHASE_INC_20000, PHASE_INC_20000, 120000);
      break;
    case SINUS_836HZ_856HZ_60S:
      init_constant_tone(PHASE_INC_836, PHASE_INC_856, 60000);
      break;
    case SINUS_ALARM_90S:
      init_cycle_tone(PHASE_INC_250, PHASE_INC_500, PHASE_INC_300, 3000, 84000);
      break;
    case SINUS_ALL_CLEAR_60S:
      init_cycle_tone(PHASE_INC_250, PHASE_INC_425, 0, 2000, 56000);
      break;
    case SINUS_ALL_CLEAR_120S:
      init_cycle_tone(PHASE_INC_250, PHASE_INC_425, 0, 2000, 116000);
      break;
    case SINUS_ABC_120S:
      init_cycle_tone(PHASE_INC_250, PHASE_INC_500, PHASE_INC_300, 500, 89000);
      break;
    case SINUS_400HZ_800HZ_1_5S_60S:
      init_cycle_tone(PHASE_INC_400, PHASE_INC_800, PHASE_INC_400, 1500, 56000);
      break;
    case SINUS_400HZ_800HZ_3S_60S:
      init_cycle_tone(PHASE_INC_400, PHASE_INC_800, PHASE_INC_400, 3000, 54000);
      break;
    default:
      set_increment(PHASE_INC_100, PHASE_INC_100);
      break;
  }

  inc_data.total_samples_generated = 0;
  inc_data.total_samples_fade = 0;
  inc_data.ramp_counter = 0;
  inc_data.fade_stop = false;

  phase_acc_ch1 = 0;
  phase_acc_ch2 = 0;
}

void set_increment (uint32_t value_ch1, uint32_t value_ch2)
{
  phase_inc_ch1 = value_ch1;
  phase_inc_ch2 = value_ch2;
}

static void init_constant_tone (uint32_t freq_left_inc,
				uint32_t freq_right_inc,
				uint32_t duration_ms)
{
  inc_data.samples_per_phase[2] = (uint64_t) SAMPLE_RATE * duration_ms / 1000;
  inc_data.duration = inc_data.samples_per_phase[2];

  inc_data.current_step = 1;
  inc_data.stable_middle = true;
  inc_data.stable_freq_phase = freq_left_inc;

  inc_data.current_inc = freq_left_inc;
  inc_data.target_inc = freq_left_inc;
  inc_data.add_inc[0] = inc_data.add_inc[1] = 0;

  phase_inc_ch1 = freq_left_inc;
  phase_inc_ch2 = freq_right_inc;
}

static void init_cycle_tone (uint32_t start_inc,
			     uint32_t end_inc,
			     uint32_t mid_inc,
			     uint32_t ending_ms,
			     uint32_t mid_ms)
{
  inc_data.target_phase[0] = start_inc;
  inc_data.target_phase[1] = end_inc;
  inc_data.target_phase[2] = mid_inc;

  inc_data.samples_per_phase[0] = (uint64_t) SAMPLE_RATE * ending_ms / 1000;
  inc_data.samples_per_phase[1] = (uint64_t) SAMPLE_RATE * (ending_ms / 2) / 1000;
  inc_data.samples_per_phase[2] = (uint64_t) SAMPLE_RATE * mid_ms / 1000;

  inc_data.duration = (inc_data.samples_per_phase[0] * 2) + inc_data.samples_per_phase[2];

  inc_data.current_step = 0;
  inc_data.stable_freq_phase = end_inc;
  inc_data.stable_middle = mid_inc == 0;

  inc_data.current_inc = start_inc;
  inc_data.target_inc = end_inc;
  inc_data.add_inc[0] = (end_inc - start_inc) / ((SAMPLE_RATE * ending_ms) / 1000);
  inc_data.add_inc[1] = (end_inc - mid_inc) / ((SAMPLE_RATE * (ending_ms / 2)) / 1000);

  phase_inc_ch1 = phase_inc_ch2 = start_inc;
}

void set_fade_stop ()
{
  inc_data.total_samples_fade = inc_data.total_samples_generated + FADE_IN_SAMPLE;
  inc_data.fade_stop = true;
}

static void apply_fade (int16_t *left, int16_t *right, uint64_t sample_idx)
{
  if (sample_idx < FADE_IN_SAMPLE)
  {
    uint32_t count = (uint32_t) (sample_idx * FADE_IN_STEP >> 10);
    if (count > 32767)
      count = 32767;
    *left = (count * *left) >> 15;
    *right = (count * *right) >> 15;
  }
}

static void apply_fade_out (int16_t *left, int16_t *right, uint64_t remaining)
{
  uint32_t count = (uint32_t) (remaining * FADE_IN_STEP >> 10);
  if (count > 32767)
    count = 32767;
  *left = (count * *left) >> 15;
  *right = (count * *right) >> 15;
}

static void generate_sample (uint8_t *buffer,
			     uint32_t idx,
			     int16_t left,
			     int16_t right)
{
  buffer[idx] = left & 0xFF;
  buffer[idx + 1] = (left >> 8) & 0xFF;
  buffer[idx + 2] = right & 0xFF;
  buffer[idx + 3] = (right >> 8) & 0xFF;
}

static void update_phase_and_increment (void)
{
  phase_acc_ch1 += phase_inc_ch1;
  phase_acc_ch2 += phase_inc_ch2;

  if (inc_data.current_inc < inc_data.target_inc)
  {
    inc_data.current_inc += inc_data.add_inc[inc_data.current_step == 1 ? 1 : 0];
    if (inc_data.current_inc > inc_data.target_inc)
      inc_data.current_inc = inc_data.target_inc;
  }
  else if (inc_data.current_inc > inc_data.target_inc)
  {
    inc_data.current_inc -= inc_data.add_inc[inc_data.current_step == 1 ? 1 : 0];
    if (inc_data.current_inc < inc_data.target_inc)
      inc_data.current_inc = inc_data.target_inc;
  }

  phase_inc_ch1 = inc_data.current_inc;
  phase_inc_ch2 = inc_data.current_inc;
}

static bool handle_ramp_transition (uint8_t *buffer,
				    Audio_Player_t *player,
				    uint32_t samples_per_channel,
				    uint32_t current_sample_in_buffer)
{
  uint64_t ending_ramp = inc_data.samples_per_phase[0];
  uint64_t cycle_ramp = inc_data.samples_per_phase[1];
  uint64_t middle_ramp = inc_data.samples_per_phase[2];

  uint64_t samples_per_ramp =
      (inc_data.current_step == 1 && !inc_data.stable_middle) ? cycle_ramp : ending_ramp;

  if (++inc_data.ramp_counter < samples_per_ramp)
    return false;

  inc_data.ramp_counter = 0;

  switch (inc_data.current_step)
  {
    case STEP_RAMP_UP:
      inc_data.current_step = STEP_CYCLE;
      inc_data.target_inc =
	  inc_data.stable_middle ? inc_data.stable_freq_phase : inc_data.target_phase[2];
      break;

    case STEP_CYCLE:
      if (!inc_data.stable_middle)
      {
	inc_data.target_inc =
	    (inc_data.target_inc == inc_data.target_phase[2]) ? inc_data.target_phase[1] : inc_data.target_phase[2];
      }

      if (inc_data.total_samples_generated - ending_ramp >= middle_ramp)
      {
	inc_data.current_step = STEP_RAMP_DOWN;
	inc_data.target_inc = inc_data.target_phase[0];
      }
      break;

    case STEP_RAMP_DOWN:
    {
      uint32_t bytes_to_clear = (samples_per_channel - current_sample_in_buffer - 1) * 4;
      if (bytes_to_clear > 0)
      {
	memset(buffer + (current_sample_in_buffer + 1) * 4, 0, bytes_to_clear);
      }
      player->duration = 100;
      player->is_prepare_stoped = true;
      return true;
    }
  }
  return false;
}
