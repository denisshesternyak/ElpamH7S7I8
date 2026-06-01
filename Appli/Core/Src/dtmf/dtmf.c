#include "dtmf.h"
#include <math.h>
#include "main.h"
#include "gpio.h"
#include "audio.h"
#include "logger.h"
#include "app_freertos.h"
#include "events.h"
#include <stdio.h>

//static int16_t samples[DTMF_BUFFER_RX_SIZE];
static int32_t power_all[DTMF_COUNT_NUM];
static int32_t coeff[DTMF_COUNT_NUM];
static const uint16_t f_tone[DTMF_COUNT_NUM] = { 697, 770, 852, 941, 1209, 1336, 1477, 1633 };
static const char row_col[DTMF_HALF_COUNT_NUM][DTMF_HALF_COUNT_NUM] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

#define FS 8000.0f

static int32_t goertzel (const int16_t *sample, int32_t coeff);
static void post_test (void);

static void dtmf_start (void);
static void dtmf_check (DTMFMessage_t *msg);
static void dtmf_stop (void);

//void sin_data (int f_row, int col)
//{
//  int16_t amp = 512;
//  for (int i = 0; i < DTMF_BUFFER_RX_SIZE; i++)
//  {
//    int16_t sin1 = sin (2 * M_PI * (i * f_row / FS)) * amp;
//    int16_t sin2 = sin (2 * M_PI * (i * col / FS)) * amp;
//    int16_t sinsum = sin1 + sin2;
//
//    samples[i] = sinsum;
//  }
//
//  for (uint8_t i = 0; i < DTMF_COUNT_NUM; i++)
//    power_all[i] = goertzel (samples, coeff[i]);
//
//  post_test ();
//}

void dtmf_init (void)
{
  for (uint8_t i = 0; i < DTMF_COUNT_NUM; i++)
  {
    coeff[i] = (int32_t) (2.0 * cos(2.0 * M_PI * f_tone[i] / FS) * (1 << 14));
  }

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
  LOG_DEBUG("DTMF Start");
  audio_notify_medium(AUDIO_START, AUDIO_DTMF);
}

static void dtmf_check (DTMFMessage_t *msg)
{
//  LOG_DEBUG("DTMF Check");

  for (uint8_t i = 0; i < DTMF_COUNT_NUM; i++)
  {
    power_all[i] = goertzel((int16_t*) msg->data, coeff[i]);
  }

//  static bool fl = false;
//
//  if(!fl)
//  {
//    printf("buff:\r\n");
//    for(uint8_t j = 0; j < 20; j++)
//    {
//      printf("%04x ", msg->data[j]);
//    }
//    printf("\r\n");
//    fl = true;
//  }

  for(uint8_t j = 0; j < 30; j++)
  {
    printf("%04x ", msg->data[j]);
  }
  printf("\r\n\r\n");



  post_test();

  osDelay(1000);
  audio_notify_medium(AUDIO_PLAY, AUDIO_DTMF);
}

static void dtmf_stop (void)
{
  LOG_DEBUG("DTMF Stop");
  audio_notify_medium(AUDIO_STOP, AUDIO_DTMF);
}

static int32_t goertzel (const int16_t *sample, int32_t coeff)
{
  int32_t Q, Q_prev, Q_prev2, i;
  int64_t prod1, prod2, prod3, power;

  Q_prev = 0;
  Q_prev2 = 0;
  power = 0;

  for (i = 0; i < DTMF_BUFFER_RX_SIZE; i++)
    {
      Q = (sample[i]) + ((coeff * Q_prev) >> 14) - (Q_prev2);
      Q_prev2 = Q_prev;
      Q_prev = Q;
    }

  prod1 = ((long) Q_prev * Q_prev);
  prod2 = ((long) Q_prev2 * Q_prev2);
  prod3 = ((long) Q_prev * coeff) >> 14;
  prod3 = (prod3 * Q_prev2);

  power = ((prod1 + prod2 - prod3)) >> 8;

  return power;
}

static void post_test (void)
{
  int32_t row = -1, col = -1;
  int32_t max_row = 0, max_col = 0;

  for (int i = 0; i < DTMF_HALF_COUNT_NUM; i++)
  {
    if (power_all[i] > max_row)
    {
      max_row = power_all[i];
      row = i;
    }
  }

  for (int i = 4; i < 8; i++)
  {
    if (power_all[i] > max_col)
    {
      max_col = power_all[i];
      col = i - 4;
    }
  }

  // printf("Powers: %u %u %u %u | %u %u %u %u\n",
  //       power_all[0], power_all[1], power_all[2], power_all[3],
  //       power_all[4], power_all[5], power_all[6], power_all[7]);

  char digit = row_col[row][col];
  LOG_DEBUG("digit: %c (power: %u / %u)\n", digit, max_row, max_col);
}
