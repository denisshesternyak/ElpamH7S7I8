#include <audio.h>
#include "math.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "string.h"
#include "audio_cmd.h"
#include "audio_regs.h"
#include "audio_generate_sin.h"
#include "sdfs.h"
#include "lcd_menu.h"
#include "system_status.h"
#include "logger.h"
#include "i2s.h"
#include "app_freertos.h"
#include "dtmf_types.h"
#include <math.h>

#define CODEC_I2S_HANDLER	&hi2s6

Audio_Player_t player;

static uint8_t dma_buffer_tx[AUDIO_BUFFER_SIZE] __attribute__((section(".extram")));
static int16_t dma_buffer_rx[DTMF_BUFFER_RX_SIZE] __attribute__((section(".extram")));

// List of acceptable levels
const uint8_t valid_volume_levels[] = {
    80,
    83,
    86,
    89,
    92,
    95,
    98,
    101,
    104,
    107,
    110,
    113,
    116,
    119,
    122 };

static void audio_event (AudioNotify_t *audio_notify);

// PROCESS
static void audio_start (AudioNotify_t *audio_notify);
static void audio_play (AudioNotify_t *audio_notify);
static void audio_stop (AudioNotify_t *audio_notify);
static void audio_prepare_stop (AudioNotify_t *audio_notify);
static void audio_pause (void);
static void audio_timer (void);
static void audio_volume (AudioVolume_t *volume);
static void audio_arming (bool val);

// SINUS
static void audio_start_sinus (AudioNotify_t *audio_notify);
static void audio_play_sinus (void);
static void audio_stop_sinus (void);
static void audio_prepare_stop_sinus (void);

// SD
static void audio_start_sd (AudioNotify_t *audio_notify);
static void audio_play_sd (void);
static void audio_stop_sd (void);
static void audio_prepare_stop_sd ();

// MIC
static void audio_start_mic (void);
static void audio_play_mic (void);
static void audio_stop_mic (void);
static void audio_prepare_stop_mic (void);

// MOTOROLA
static void audio_start_motorola (void);
static void audio_play_motorola (void);
static void audio_stop_motorola (void);
static void audio_prepare_stop_motorola (void);

// DTMF
static void audio_start_dtmf (void);
static void audio_play_dtmf (void);
static void audio_stop_dtmf (void);
static void audio_prepare_stop_dtmf (void);

// QUIET
static void audio_start_quiet (AudioNotify_t *audio_notify);
static void audio_stop_quiet (void);

static void start_playback (void);
static void stop_playback (void);
static void check_progress (void);
static void send_audio_notify_playing (BufferState_t buff_state);
static osStatus_t send_audio_notify (AudioNotify_t *audio_notify,
				     AudioEvent_t event,
				     AudioType_t type,
				     AudioPriority_t priority,
				     uint32_t timeout);

void audio_init (void)
{
  HAL_GPIO_WritePin(CODEC_RESET_GPIO_Port, CODEC_RESET_Pin, GPIO_PIN_RESET);
  osDelay(10);
  HAL_GPIO_WritePin(CODEC_RESET_GPIO_Port, CODEC_RESET_Pin, GPIO_PIN_SET);
  osDelay(10);

  memset(&player, 0, sizeof(player));
//  memcpy(player.valid_volume_levels, valid_volume_levels, NUM_VALID_LEVELS);

  player.volume_level = 15;
  if (player.volume_level < 1)
    player.volume_level = 1;
  player.volume_value = valid_volume_levels[player.volume_level - 1];
//    player.volume_value = DEF_VALUE_VOLUME;

  audio_init_sin_table();

  osDelay(10);
  audio_cmd_reset();
  osDelay(50);

  audio_cmd_init_power();
  audio_cmd_init_playback();
  audio_cmd_init_record();
  audio_set_volume_playback(CUR_VOLUME_PLAYBACK);
}

void audio_process (AudioNotify_t *audio_notify)
{
  switch (audio_notify->event)
  {
    case AUDIO_IDLE:
      break;
    case AUDIO_ARMIG:
      audio_arming(audio_notify->arming);
      break;
    case AUDIO_TIMER:
      audio_timer();
      break;
    case AUDIO_VOLUME:
      audio_volume(&audio_notify->volume);
      break;
    default:
      audio_event(audio_notify);
      break;
  }

  player.event = AUDIO_IDLE;
}

static void audio_event (AudioNotify_t *audio_notify)
{
  if (audio_notify->priority < player.priority)
    return;
  player.priority = audio_notify->priority;
  if(audio_notify->sample.type != AUDIO_CURRENT_TYPE)
    player.type = audio_notify->sample.type;

  switch (audio_notify->event)
  {
    case AUDIO_START:
      audio_start(audio_notify);
      break;
    case AUDIO_PLAY:
      audio_play(audio_notify);
      break;
    case AUDIO_STOP:
      audio_stop(audio_notify);
      break;
    case AUDIO_PREPARE_STOP:
      audio_prepare_stop(audio_notify);
      break;
    case AUDIO_PAUSE:
      audio_pause();
      break;
    default:
      break;
  }
}

static void audio_start (AudioNotify_t *audio_notify)
{
//	LOG_DEBUG("AUDIO_START");

  switch (player.type)
  {
    case AUDIO_SIN:
      audio_start_sinus(audio_notify);
      break;
    case AUDIO_SD:
      audio_start_sd(audio_notify);
      break;
    case AUDIO_MIC:
      audio_start_mic();
      break;
    case AUDIO_MOTOROLA:
      audio_start_motorola();
      break;
    case AUDIO_DTMF:
      audio_start_dtmf();
      break;
    case AUDIO_QUIET:
      audio_start_quiet(audio_notify);
    default:
      break;
  }
}

static void audio_play (AudioNotify_t *audio_notify)
{
//	LOG_DEBUG("AUDIO_PLAY");

  switch (player.type)
  {
    case AUDIO_SIN:
      audio_play_sinus();
      break;
    case AUDIO_SD:
      audio_play_sd();
      break;
    case AUDIO_MIC:
      audio_play_mic();
      break;
    case AUDIO_MOTOROLA:
      audio_play_motorola();
      break;
    case AUDIO_DTMF:
      audio_play_dtmf();
      break;
    default:
      break;
  }
}

static void audio_stop (AudioNotify_t *audio_notify)
{
//	LOG_DEBUG("AUDIO_STOP");

  switch (player.type)
  {
    case AUDIO_SIN:
      audio_stop_sinus();
      break;
    case AUDIO_SD:
      audio_stop_sd();
      break;
    case AUDIO_MIC:
      audio_stop_mic();
      break;
    case AUDIO_MOTOROLA:
      audio_stop_motorola();
      break;
    case AUDIO_DTMF:
      audio_stop_dtmf();
      break;
    case AUDIO_QUIET:
      audio_stop_quiet();
    default:
      break;
  }
}

static void audio_prepare_stop (AudioNotify_t *audio_notify)
{
//	LOG_DEBUG("AUDIO_PREPARE_STOP");

  switch (player.type)
  {
    case AUDIO_SIN:
      audio_prepare_stop_sinus();
      break;
    case AUDIO_SD:
      audio_prepare_stop_sd();
      break;
    case AUDIO_MIC:
      audio_prepare_stop_mic();
      break;
    case AUDIO_MOTOROLA:
      audio_prepare_stop_motorola();
      break;
    case AUDIO_DTMF:
      audio_prepare_stop_dtmf();
      break;
    default:
      break;
  }
}

static void audio_pause (void)
{

}

static void audio_arming (bool val)
{
  player.last_time_arming = 0;
  player.is_arming = val;
}

static void audio_timer (void)
{
  if (player.is_arming)
  {
    player.last_time_arming++;
    player.is_arming = player.last_time_arming < ARMING_TIME;
    if (!player.is_arming)
    {
      player.last_time_arming = 0;
      LOG_DEBUG("ARM time's up");
    }
  }

  if (player.is_announcement)
  {
    player.last_time_announcement++;
    player.is_announcement = player.last_time_announcement < ANNOUNCEMENT_TIME;
    if (!player.is_announcement)
    {
      player.last_time_announcement = 0;
      LOG_DEBUG("ANNOUNCEMENT time's up");
      LCDTaskEvent_t lcd_event = { .event = LCD_EVENT_BTN, .btn = {
	  .button = BTN_ESC,
	  .pressed = true } };
      xQueueSend(xLCDQueueHandle, &lcd_event, portMAX_DELAY);
      audio_notify_low(AUDIO_STOP, AUDIO_MIC);
    }
  }

  if (player.is_recording)
  {
    player.last_time_recording++;
    player.is_recording = player.last_time_recording < RECORDING_TIME;
    if (!player.is_recording)
    {
      player.is_recording = false;
      player.last_time_recording = 0;
      LOG_DEBUG("DTMF listening time's up");

      DTMFMessage_t msg;
      msg.event = DTMF_STOP;
      msg.data = NULL;
      osMessageQueuePut(xDTMFQueueHandle, &msg, 0, 0);
    }
  }
}

static void audio_volume (AudioVolume_t *volume)
{
  if (player.is_playing || !player.is_announcement)
    return;

  switch(volume->event)
  {
    case VOLUME_INCREASE:
      if(player.volume_level >= NUM_VALID_LEVELS)
	return;
      player.volume_level++;
      break;
    case VOLUME_DECREASE:
      if(player.volume_level == 1)
	return;
      player.volume_level--;
      break;
    default:
      break;
  }

  player.volume_value = valid_volume_levels[player.volume_level - 1];
  audio_set_volume(player.volume_value);
  volume->handler(player.volume_level, player.volume_value);
}

static int audio_find_closest_valid_volume (uint8_t target)
{
  if (target <= MIN_VOLUME)
    return MIN_VOLUME;
  if (target >= MAX_VOLUME)
    return MAX_VOLUME;

  for (int i = 0; i < NUM_VOLUME_BARS; i++)
  {
    if (valid_volume_levels[i] >= target)
    {
      player.volume_level = i + 1;
      return valid_volume_levels[i];
    }
  }
  return MAX_VOLUME;
}

void audio_set_volume (uint8_t value)
{
  uint8_t corrected_vol = audio_find_closest_valid_volume(value);

  player.volume_value = corrected_vol;

  uint8_t vol;
  if (corrected_vol >= MAX_VOLUME)
    vol = MAX_VOLUME_CODEC;
  else if (corrected_vol <= MIN_VOLUME)
    vol = MIN_VOLUME_CODEC;
  else
    vol = CNVR_VOL(corrected_vol);

  audio_cmd_send_volume_announc(vol);

  system_status.max_volume = (corrected_vol == MAX_VOLUME);

  //uint8_t bar_index = find_volume_index(corrected) + 1;
  //VolumeIndicator_SetLevelSilent(&volumeIndicator, bar_index);
}

void audio_set_volume_playback (uint8_t level)
{
  uint8_t vol = 0;
  if (level == 0) vol = MAX_VOLUME_PLAYBACK;
  else if (level >= 150) vol = MIN_VOLUME_PLAYBACK;
  else vol = (uint8_t)(129 + (level * 127 / 150));

  audio_cmd_send_volume_dac(vol);
}

void audio_get_volume_level(uint8_t *level, uint8_t *value)
{
  *level = player.volume_level;
  *value = player.volume_value;
}

// SINUS
static void audio_start_sinus (AudioNotify_t *audio_notify)
{
  LOG_INFO("Start playback sinus");

  init_generation(audio_notify->sample.sin_task);
  audio_generate_sine(&player, dma_buffer_tx, AUDIO_STEREO_PAIRS_FULL);

  audio_cmd_playback_enable();

  start_playback();
}

static void audio_play_sinus (void)
{
  check_progress();

  if (player.is_prepare_stoped)
  {
    player.is_stoped = true;
    return;
  }

  uint32_t offset =
      (player.buff_state == BUFFER_HALF) ? 0 : AUDIO_HALF_BUFFER_SIZE;
  uint8_t *buf_ptr = dma_buffer_tx + offset;

  audio_generate_sine(&player, buf_ptr, AUDIO_STEREO_PAIRS_HALF);

  player.buff_state = BUFFER_IDLE;
  player.event = AUDIO_PLAY;
}

static void audio_stop_sinus (void)
{
  stop_playback();
  audio_cmd_playback_disable();

  LOG_INFO("Stop playback sinus");
}

static void audio_prepare_stop_sinus (void)
{
  if (player.is_playing && !player.is_stoped)
  {
    player.event = AUDIO_PLAY;
    set_fade_stop();
  }
}

// SD
static void audio_start_sd (AudioNotify_t *audio_notify)
{
  if (!audio_notify->sample.filename)
    return;

  player.file_info.filename = audio_notify->sample.filename;

  LOG_INFO("Start playback from uSD: %s", player.file_info.filename);

  bool res = sdfs_read_file_info(&player.file_info);
  if (!res)
  {
    LOG_ERROR("Failure load info: %s", player.file_info.filename);
    return;
  }

  res = sdfs_read_file(&player.file_info, dma_buffer_tx, AUDIO_BUFFER_SIZE);
  if (!res)
  {
    LOG_ERROR("Failure load data: %s", player.file_info.filename);
    return;
  }

  audio_cmd_playback_enable();

  start_playback();
}

static void audio_play_sd (void)
{
  check_progress();

  uint32_t offset =
      (player.buff_state == BUFFER_HALF) ? 0 : AUDIO_HALF_BUFFER_SIZE;
  uint8_t *buf_ptr = dma_buffer_tx + offset;

  bool res = sdfs_read_file(&player.file_info, buf_ptr, AUDIO_HALF_BUFFER_SIZE);
  if (player.file_info.isEnd || !res)
  {
    player.is_stoped = true;
    player.duration = 100;
    check_progress();
  }

  player.buff_state = BUFFER_IDLE;
  player.event = AUDIO_PLAY;

  player.duration = (player.file_info.position * 100) / player.file_info.file_size;
}

static void audio_stop_sd (void)
{
  stop_playback();
  audio_cmd_playback_disable();

  sdfs_close_audiofile(&player.file_info);
  memset(&player.file_info, 0, sizeof(player.file_info));

  LOG_INFO("Stop playback from uSD");
}

static void audio_prepare_stop_sd (void)
{
  if (player.is_playing && !player.is_stoped)
  {
    player.event = AUDIO_PLAY;
    player.is_stoped = true;
    player.is_fade_stoped = true;
  }
}

// MIC
static void audio_start_mic (void)
{
  LOG_INFO("Start playback announcement");

//	audio_cmd_playback_enable();
  player.last_time_announcement = 0;
  player.is_announcement = true;
  audio_cmd_IN1R_enable();
}

static void audio_play_mic (void)
{

}

static void audio_stop_mic (void)
{
//	audio_cmd_playback_disable();
  player.is_announcement = false;
  audio_cmd_IN1R_disable();

  LOG_INFO("Stop playback announcement");
}

static void audio_prepare_stop_mic (void)
{

}

// MOTOROLA
static void audio_start_motorola (void)
{
  player.is_motorola = true;
  audio_cmd_IN1R_enable();

  LOG_INFO("Start playback motorola");
}

static void audio_play_motorola (void)
{

}

static void audio_stop_motorola (void)
{
  player.is_motorola = false;
  player.event = AUDIO_IDLE;
  player.priority = AUDIO_PRIORITY_IDLE;

  audio_cmd_IN1R_disable();

  LOG_INFO("Stop playback motorola");
}

static void audio_prepare_stop_motorola (void)
{

}

// DTMF
static void audio_start_dtmf (void)
{
  LOG_INFO("Start record DTMF");
//  audio_cmd_I2S_to_DAC();

  player.is_recording = true;
  player.is_stoped = false;
//  player.is_prepare_stoped = false;
//  player.is_fade_stoped = false;
  player.event = AUDIO_PLAY;
  player.last_time_recording = 0;

//  int16_t amp = 512;
//  for (int i = 0; i < DTMF_BUFFER_RX_SIZE; i++)
//  {
//    int16_t sin1 = sin (2 * M_PI * (i * 697 / 8000.0f)) * amp;
//    int16_t sin2 = sin (2 * M_PI * (i * 1477 / 8000.0f)) * amp;
//    int16_t sinsum = sin1 + sin2;
//
//    dma_buffer_rx[i] = sinsum;
//  }

  hi2s6.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s6.Init.Mode = I2S_MODE_MASTER_RX;
  hi2s6.Init.AudioFreq = I2S_AUDIOFREQ_8K;
  HAL_I2S_Init(CODEC_I2S_HANDLER);
  HAL_I2S_Receive_DMA(CODEC_I2S_HANDLER, (uint16_t *)dma_buffer_rx, DTMF_BUFFER_RX_SIZE);
}

static void audio_play_dtmf (void)
{
//  uint32_t offset =
//      (player.buff_state == BUFFER_HALF) ? 0 : AUDIO_HALF_BUFFER_RX_SIZE;
//  uint8_t *buf_ptr = dma_buffer_tx + offset;
//
//  audio_generate_sine(&player, buf_ptr, AUDIO_STEREO_PAIRS_HALF);

//  LOG_INFO("Play record DTMF");
//
//  static bool fl = false;
//  uint16_t row = fl ? 941 : 697;
//  uint16_t col = fl ? 1633 : 1477;
//
//  fl = !fl;
//
//  int16_t amp = 512;
//
//  for (int i = 0; i < DTMF_BUFFER_RX_SIZE; i++)
//  {
//    int16_t sin1 = sin (2 * M_PI * (i * row / 8000.0f)) * amp;
//    int16_t sin2 = sin (2 * M_PI * (i * col / 8000.0f)) * amp;
//    int16_t sinsum = sin1 + sin2;
//
//    dma_buffer_rx[i] = sinsum;
//  }
//
//  player.buff_state = BUFFER_IDLE;
//  player.event = AUDIO_PLAY;
//
//  hi2s6.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
//  hi2s6.Init.Mode = I2S_MODE_MASTER_RX;
//  hi2s6.Init.AudioFreq = I2S_AUDIOFREQ_8K;
//  HAL_I2S_Init(CODEC_I2S_HANDLER);
//  HAL_I2S_Receive_DMA(CODEC_I2S_HANDLER, (uint16_t *)dma_buffer_tx, DTMF_BUFFER_RX_SIZE);

  hi2s6.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s6.Init.Mode = I2S_MODE_MASTER_RX;
  hi2s6.Init.AudioFreq = I2S_AUDIOFREQ_8K;
  HAL_I2S_Init(CODEC_I2S_HANDLER);
  HAL_I2S_Receive_DMA(CODEC_I2S_HANDLER, (uint16_t *)dma_buffer_rx, DTMF_BUFFER_RX_SIZE);

}

static void audio_stop_dtmf (void)
{
  LOG_INFO("Stop record DTMF");

  if (!player.is_recording)
    return;

  player.is_recording = false;
  player.is_stoped = true;
  player.event = AUDIO_IDLE;
  player.buff_state = BUFFER_IDLE;
  player.priority = AUDIO_PRIORITY_IDLE;

  HAL_I2S_DMAStop(CODEC_I2S_HANDLER);
  hi2s6.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  HAL_I2S_Init(CODEC_I2S_HANDLER);
}

static void audio_prepare_stop_dtmf (void)
{

}

// QUIET
static void audio_start_quiet (AudioNotify_t *audio_notify)
{
  LOG_INFO("Start playback quiet test");

  init_generation(audio_notify->sample.sin_task);
  audio_generate_sine(&player, dma_buffer_tx, AUDIO_STEREO_PAIRS_FULL);

  audio_cmd_quiet_enable();
  start_playback();
}

static void audio_stop_quiet (void)
{
  audio_cmd_quiet_disable();
  stop_playback();

  LOG_INFO("Stop playback quiet test");
}

////
static void start_playback (void)
{
//	if (player.priority > player.current_priority && player.is_playing)
//	{
//		HAL_I2S_DMAStop(CODEC_I2S_HANDLER);
//	}

  audio_cmd_I2S_to_DAC();

  player.is_playing = true;
  player.is_stoped = false;
  player.is_prepare_stoped = false;
  player.is_fade_stoped = false;
  player.event = AUDIO_PLAY;

  hi2s6.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s6.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s6.Init.AudioFreq = I2S_AUDIOFREQ_44K;
  HAL_I2S_Init(CODEC_I2S_HANDLER);
  HAL_I2S_Transmit_DMA(CODEC_I2S_HANDLER, (uint16_t*) dma_buffer_tx, AUDIO_HALF_BUFFER_SIZE);

//	audio_set_volume(player.volume);
}

static void stop_playback (void)
{
  if (!player.is_playing)
    return;
  player.is_playing = false;
  player.is_stoped = true;
  player.duration = 0;
  player.event = AUDIO_IDLE;
  player.buff_state = BUFFER_IDLE;
  player.priority = AUDIO_PRIORITY_IDLE;

//	LOG_DEBUG("STOP");

  if (!player.is_fade_stoped)
  {
    LCDTaskEvent_t lcd_event = { .event = LCD_EVENT_BTN, .btn = {
	.button = BTN_ESC,
	.pressed = true } };
    xQueueSend(xLCDQueueHandle, &lcd_event, portMAX_DELAY);
  }

  HAL_I2S_DMAStop(CODEC_I2S_HANDLER);
  hi2s6.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  HAL_I2S_Init(CODEC_I2S_HANDLER);
}

static void check_progress (void)
{
  static uint8_t update_progree = 0;
  if ((update_progree++ > COUNT_PROGRESS) || player.is_prepare_stoped)
  {
    update_progree = 0;
    LCDTaskEvent_t lcd_event = {
	.event = LCD_EVENT_PROGRESS,
	.value = player.duration };
    xQueueSend(xLCDQueueHandle, &lcd_event, portMAX_DELAY);
  }
}

void HAL_I2S_TxHalfCpltCallback (I2S_HandleTypeDef *hi2s)
{
  if (hi2s->Instance != SPI6)
    return;
  send_audio_notify_playing(BUFFER_HALF);
}

void HAL_I2S_TxCpltCallback (I2S_HandleTypeDef *hi2s)
{
  if (hi2s->Instance != SPI6)
    return;
  send_audio_notify_playing(BUFFER_FULL);
}

//static bool fl = false;

//void HAL_I2S_RxHalfCpltCallback (I2S_HandleTypeDef *hi2s)
//{
//  if (hi2s->Instance != SPI6)
//    return;
//
//  if (!player.is_recording || player.event == AUDIO_STOP)
//      return;
//
////  if(!fl)
////  {
////    memcpy(dma_buffer_rx, dtmf_test_digit_1, 100);
////    for(int j = 0; j < 100; j++)
////    {
////      printf("%04x ", dma_buffer_rx[j]);
////    }
////    printf("---\r\n");
////    fl = true;
////  }
//
//  DTMFMessage_t msg;
//  msg.event = DTMF_CHECK;
//  msg.data = dma_buffer_rx;
//
//  osMessageQueuePut(xDTMFQueueHandle, &msg, 0, 0);
//}

void HAL_I2S_RxCpltCallback (I2S_HandleTypeDef *hi2s)
{
  if (hi2s->Instance != SPI6)
    return;

  if (!player.is_recording || player.event == AUDIO_STOP)
      return;

  DTMFMessage_t msg;
  msg.event = DTMF_CHECK;
  msg.data = dma_buffer_rx;

  osMessageQueuePut(xDTMFQueueHandle, &msg, 0, 0);
}

static osStatus_t send_audio_notify (AudioNotify_t *audio_notify,
				     AudioEvent_t event,
				     AudioType_t type,
				     AudioPriority_t priority,
				     uint32_t timeout)
{
  audio_notify->event = event;
  audio_notify->sample.type = type;
  audio_notify->priority = priority;
  return osMessageQueuePut(xAudioQueueHandle, audio_notify, 0, timeout);
}

static void send_audio_notify_playing (BufferState_t buff_state)
{
  if (!player.is_playing || player.event == AUDIO_STOP)
    return;

  player.buff_state = buff_state;

  AudioNotify_t audio_notify;
  AudioEvent_t event = player.is_stoped ? AUDIO_STOP : AUDIO_PLAY;

  if (send_audio_notify(&audio_notify, event, player.type, player.priority, 0) != osOK)
  {
    LOG_WARN("The audio queue is full");
  }
}

void audio_notify_low (AudioEvent_t event, AudioType_t type)
{
  AudioNotify_t audio_notify;

  if (send_audio_notify(&audio_notify, event, type, AUDIO_PRIORITY_LOW, 10) != osOK)
  {
    LOG_WARN("The event from KEYBOARD cannot be handled. The audio queue is full");
  }
}

void audio_notify_start_task_low (AudioType_t type,
				  SinTask_t task,
				  const char *name)
{
  AudioNotify_t audio_notify = { .sample.sin_task = task, .sample.filename = name };

  if (send_audio_notify(&audio_notify, AUDIO_START, type, AUDIO_PRIORITY_LOW, 10) != osOK)
  {
    LOG_WARN("The event from KEYBOARD cannot be handled. The audio queue is full");
  }
}

void audio_notify_stop_task_low (AudioType_t type,
				  SinTask_t task,
				  const char *name)
{
  AudioNotify_t audio_notify = { .sample.sin_task = task, .sample.filename = name };

  if (send_audio_notify(&audio_notify, AUDIO_STOP, type, AUDIO_PRIORITY_LOW, 10) != osOK)
  {
    LOG_WARN("The event from KEYBOARD cannot be handled. The audio queue is full");
  }
}

void audio_notify_high (AudioEvent_t event, AudioType_t type)
{
  AudioNotify_t audio_notify;

  if (send_audio_notify(&audio_notify, event, type, AUDIO_PRIORITY_HIGH, 10) != osOK)
  {
    LOG_WARN("The event from MOTOROLA cannot be handled. The audio queue is full");
  }
}

void audio_notify_medium (AudioEvent_t event, AudioType_t type)
{
  AudioNotify_t audio_notify;

  if (send_audio_notify(&audio_notify, event, type, AUDIO_PRIORITY_MEDIUM, 10) != osOK)
  {
    LOG_WARN("The event from DTMF cannot be handled. The audio queue is full");
  }
}

void audio_notify_arming (bool val)
{
  AudioNotify_t audio_notify = { .event = AUDIO_ARMIG, .priority = AUDIO_PRIORITY_LOW, .arming = val };

  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
}

bool audio_is_playing ()
{
  return player.is_playing;
}
bool audio_is_stoped ()
{
  return player.is_stoped;
}
bool audio_is_recording ()
{
  return player.is_recording;
}
bool audio_is_announcement ()
{
  return player.is_announcement;
}
bool audio_is_arming ()
{
  return player.is_arming;
}
bool audio_is_motorola ()
{
  return player.is_motorola;
}

AudioType_t audio_get_type ()
{
  return player.type;
}
