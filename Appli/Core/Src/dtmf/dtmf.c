#include "dtmf.h"
#include <math.h>
#include "main.h"
#include "gpio.h"
#include "logger.h"
#include "app_freertos.h"
#include "events.h"
#include <stdio.h>
#include "audio_types.h"
#include "lcd_types.h"
#include "defines.h"
#include "lcd_strings_menu.h"

#define DTMF_TEST_NUMBER 	0

#define FS 			48000.0f
#define PASS_COUNT 		4
#define TRASH 			50000000000LL
#define GOERTZEL_SHIFT  	14

static int16_t samples[DTMF_HALF_BUFFER_RX_SIZE];
static int64_t power_all[DTMF_COUNT_NUM];
static int32_t coeff[DTMF_COUNT_NUM];
static const uint16_t f_tone[DTMF_COUNT_NUM] = {
    697,
    770,
    852,
    941,
    1209,
    1336,
    1477,
    1633 };
static const char row_col[DTMF_HALF_COUNT_NUM][DTMF_HALF_COUNT_NUM] = {
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' } };

static bool is_pass_confirm;
static char passwordBuffer[PASS_COUNT] = { 0 };
static uint8_t passwordIndex = 0;
static const char correctPassword[] = MAINTENANCE_PASSWORD;
static bool release_btn = false;
static bool is_dtmf_running = false;

static void dtmf_clear_pass ();
static int64_t goertzel (const int16_t *sample, int32_t coeff);
static void post_test (void);
static void digit_handler (char digit);

static void dtmf_start (void);
static void dtmf_check (DTMFMessage_t *msg);
static void dtmf_stop (void);
static void notify_audio (AudioEvent_t event, AudioType_t type);
static void notify_arming (bool arming);
static void notify_audio_start_task_medium (AudioType_t type, SinTask_t task);

#if DTMF_TEST_NUMBER
#define DTMF_AMP 		15000
static void sin_data(int f_row, int f_col)
{
    for (int i = 0; i < DTMF_HALF_BUFFER_RX_SIZE; i++)
    {
        double t = (double)i / FS;

        double s1 = sin(2.0 * M_PI * f_row * t);
        double s2 = sin(2.0 * M_PI * f_col * t);

        int32_t value = (int32_t)round(
            (s1 + s2) * DTMF_AMP
        );

        samples[i] = (int16_t)value;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        power_all[i] = goertzel(samples, coeff[i]);
    }

    post_test();
    release_btn = true;
}
#endif

void dtmf_init (void)
{
  for (uint8_t i = 0; i < DTMF_COUNT_NUM; i++)
  {
    coeff[i] = (int32_t) (2.0 * cos(2.0 * M_PI * f_tone[i] / FS) * (1 << GOERTZEL_SHIFT));
  }
  release_btn = true;

//  DTMFMessage_t msg = { .event = DTMF_START, .data = NULL };
//  osMessageQueuePut(xDTMFQueueHandle, &msg, 0, 0);

//  LCDTaskEvent_t lcd_event = { .event = LCD_EVENT_SCREEN, .screen = MENU_TYPE_DTMF };
//  osMessageQueuePut(xLCDQueueHandle, &lcd_event, 0, 0);

#if DTMF_TEST_NUMBER
  osDelay(1000);
  dtmf_start();
  osDelay(1000);

  sin_data(852, 1477);  // 9
  sin_data(852, 1477);  // 9
  sin_data(852, 1477);  // 9
  sin_data(852, 1477);  // 9
  sin_data(852, 1336);  // 8
  sin_data(697, 1477);  // 3

//  sin_data(697, 1209);  // 1
//  sin_data(697, 1336);  // 2
//  sin_data(697, 1477);  // 3
//
//  sin_data(770, 1209);  // 4
//  sin_data(770, 1336);  // 5
//  sin_data(770, 1477);  // 6
//
//  sin_data(852, 1209);  // 7
//  sin_data(852, 1336);  // 8
//  sin_data(852, 1477);  // 9
//
//  sin_data(941, 1209);  // *
//  sin_data(941, 1336);  // 0
//  sin_data(941, 1477);  // #
//
//  sin_data(697, 1633);  // A
//  sin_data(770, 1633);  // B
//  sin_data(852, 1633);  // C
//  sin_data(941, 1633);  // D
#endif
}

void dtmf_process (DTMFMessage_t *msg)
{
  switch (msg->event)
  {
    case DTMF_IDLE:
      break;
    case DTMF_START:
      dtmf_start();
      break;
    case DTMF_CHECK:
      dtmf_check(msg);
      break;
    case DTMF_STOP:
      dtmf_stop();
      break;
    default:
      break;
  }
}

static void dtmf_start (void)
{
  if(is_dtmf_running) return;

  LOG_DEBUG("DTMF Start");
  is_dtmf_running = true;
  osTimerStart(RecordingTimerHandle, RECORDING_TIME);

  LCDTaskEvent_t lcd_event = { .event = LCD_EVENT_SCREEN, .screen = MENU_TYPE_DTMF };
  osMessageQueuePut(xLCDQueueHandle, &lcd_event, 0, 0);

  notify_audio(AUDIO_STOP, AUDIO_CURRENT_TYPE);
  notify_audio(AUDIO_START, AUDIO_DTMF);
}

static void dtmf_check (DTMFMessage_t *msg)
{
//  LOG_DEBUG("DTMF Check");

  if (!msg->data)
  {
    LOG_WARN("DTMFMessage_t is null");
    return;
  }

  for (size_t i = 0; i < DTMF_HALF_BUFFER_RX_SIZE; i++)
  {
    samples[i] = msg->data[2 * i + 1]; // only right channel
  }

  for (uint8_t i = 0; i < DTMF_COUNT_NUM; i++)
  {
    power_all[i] = goertzel(samples, coeff[i]);
  }

//    printf("buff:\r\n");
//    for(size_t j = 0; j < DTMF_HALF_BUFFER_RX_SIZE; j++)
//    {
//      printf("%d ", samples[j]);
//    }
//    printf("\r\n");
//    fl = true;

  post_test();

  if(!is_dtmf_running) return;

  osDelay(10);
  notify_audio(AUDIO_PLAY, AUDIO_DTMF);
}

static void dtmf_stop (void)
{
  if(!is_dtmf_running) return;
  LOG_DEBUG("DTMF Stop");
  is_dtmf_running = false;
  osTimerStop(RecordingTimerHandle);
  dtmf_clear_pass();
  notify_audio(AUDIO_STOP, AUDIO_DTMF);
}

static void dtmf_clear_pass ()
{
  is_pass_confirm = false;
  passwordIndex = 0;
  memset(passwordBuffer, 0, sizeof(passwordBuffer));
}

static int64_t goertzel (const int16_t *sample, int32_t coefficient)
{
  int64_t q0 = 0;
  int64_t q1 = 0;
  int64_t q2 = 0;

  for (int32_t i = 0; i < DTMF_HALF_BUFFER_RX_SIZE; i++)
  {
    q0 = (int64_t) sample[i] + (((int64_t) coefficient * q1) >> GOERTZEL_SHIFT) - q2;

    q2 = q1;
    q1 = q0;
  }

  int64_t p1 = q1 * q1;
  int64_t p2 = q2 * q2;

  int64_t p3 = (((int64_t) coefficient * q1) >> GOERTZEL_SHIFT) * q2;

  int64_t power = p1 + p2 - p3;

  if (power < 0)
    power = 0;

  return power;
}

static void post_test (void)
{
  int32_t row = -1, col = -1;
  int64_t max_row = 0, max_col = 0;

  for (int i = 0; i < 4; i++)
    if (power_all[i] > max_row)
    {
      max_row = power_all[i];
      row = i;
    }
  for (int i = 4; i < 8; i++)
    if (power_all[i] > max_col)
    {
      max_col = power_all[i];
      col = i - 4;
    }

  if (row < 0 || col < 0)
    return;

  if (max_row < TRASH || max_col < TRASH)
  {
    release_btn = true;
    return;
  }

  int64_t second_row = 0, second_col = 0;
  for (int i = 0; i < 4; i++)
    if (i != row && power_all[i] > second_row)
      second_row = power_all[i];
  for (int i = 4; i < 8; i++)
    if (i - 4 != col && power_all[i] > second_col)
      second_col = power_all[i];

  if (max_row < 3 * second_row || max_col < 3 * second_col)
  {
    release_btn = true;
    return;
  }

  if (!release_btn)
    return;
  release_btn = false;

  char digit = row_col[row][col];
//    LOG_DEBUG("digit: %c (power: %u / %u)\n", digit, max_row, max_col);
  LOG_DEBUG("digit: %c", digit);
  digit_handler(digit);
}

static void digit_handler (char digit)
{
  if (!is_pass_confirm)
  {
    passwordBuffer[passwordIndex++] = digit;
    if (passwordIndex >= PASS_COUNT)
    {
      bool match = (strncmp(passwordBuffer, correctPassword, PASS_COUNT) == 0);
      if (match)
      {
	is_pass_confirm = true;
	LOG_INFO("DTMF: Password confirmed");
      }
      else
      {
	LOG_WARN("DTMF: Incorrect password");
	dtmf_clear_pass();
      }
    }
    osTimerStart(RecordingTimerHandle, RECORDING_TIME);
  }
  else
  {
    if (digit == '8')
    {
      notify_arming(true);
      osTimerStart(RecordingTimerHandle, RECORDING_TIME);
    }
    else if (digit == '5')
    {
      notify_arming(false);
      dtmf_clear_pass();
    }
    else if (digit == '3')
    {
      LOG_INFO("'*A_CLR1' - got the command");
      dtmf_stop();
      notify_audio_start_task_medium(AUDIO_SIN, SINUS_ALL_CLEAR_60S);
    }
    else if (digit == '4')
    {
      LOG_INFO("'*A_CLR2' - got the command");
      dtmf_stop();
      notify_audio_start_task_medium(AUDIO_SIN, SINUS_ALL_CLEAR_120S);
    }
    else if (digit == '2')
    {
      LOG_INFO("'*CHEM_A' - got the command");
      dtmf_stop();
      notify_audio_start_task_medium(AUDIO_SIN, SINUS_CHEMICAL_90S);
    }
  }
}

static void notify_audio (AudioEvent_t event, AudioType_t type)
{
  AudioNotify_t audio_notify = {
      .event = event,
      .priority = AUDIO_PRIORITY_MEDIUM,
      .sample = { .type = type } };

  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
}

static void notify_arming (bool arming)
{
  AudioNotify_t audio_notify = {
      .event = AUDIO_ARMIG,
      .priority = AUDIO_PRIORITY_MEDIUM,
      .arming = arming };
  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
}

static void notify_audio_start_task_medium (AudioType_t type, SinTask_t task)
{
  AudioNotify_t audio_notify = {
      .event = AUDIO_START,
      .priority = AUDIO_PRIORITY_MEDIUM,
      .sample = {
	  .type = type,
	  .sin_task = task,
	  .filename = get_sinus_menu_items_str(task, 0) } };

  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
}
