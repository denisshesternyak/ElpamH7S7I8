#include "dtmf.h"
#include <math.h>
#include "main.h"
#include "gpio.h"
#include "audio.h"
#include "logger.h"
#include "app_freertos.h"
#include "events.h"

static int power_all[DTMF_COUNT_NUM];
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

static int32_t goertzel (int16_t *sample, int32_t coeff);
static void post_test (void);

static void dtmf_start (void);
static void dtmf_check (DTMFMessage_t *msg);
static void dtmf_stop (void);

void dtmf_init (void)
{
  for (int i = 0; i < DTMF_COUNT_NUM; i++)
  {
    coeff[i] = (int32_t) (2.0 * cos(2.0 * M_PI * f_tone[i] / FS) * (1 << 14));
  }
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
  LOG_DEBUG("DTMF Check");

  for (int i = 0; i < DTMF_COUNT_NUM; i++)
  {
    power_all[i] = goertzel((int16_t*) msg->data, coeff[i]);
  }

  post_test();
}

static void dtmf_stop (void)
{
  LOG_DEBUG("DTMF Stop");
  audio_notify_medium(AUDIO_STOP, AUDIO_DTMF);
}

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//    if (count < N)
//    {
//        samples[count++] = HAL_ADC_GetValue(hadc) >> 2;
//    }
//
//    if (count >= N)
//    {
//        flag = true;
//    }
//}

static int32_t goertzel (int16_t *sample, int32_t coeff)
{
  int32_t Q = 0, Q_prev = 0, Q_prev2 = 0;
  int64_t prod1, prod2, prod3, power = 0;

  for (int i = 0; i < DTMF_HALF_BUFFER_RX_SIZE; i++)
  {
    Q = sample[i] + ((coeff * Q_prev) >> 14) - Q_prev2;

    Q_prev2 = Q_prev;
    Q_prev = Q;
  }

  prod1 = (int64_t) Q_prev * Q_prev;
  prod2 = (int64_t) Q_prev2 * Q_prev2;
  prod3 = ((int64_t) Q_prev * coeff) >> 14;
  prod3 = prod3 * Q_prev2;

  power = (prod1 + prod2 - prod3) >> 8;

  return (int32_t) power;
}

static void post_test(void)
{
  int row = 0, col = 0;
  uint32_t max_power = 0;
  bool new_dig = true;

  max_power = 0;
  for (int i = 0; i < DTMF_HALF_COUNT_NUM; i++)
  {
    if (power_all[i] > max_power)
    {
      max_power = power_all[i];
      row = i;
    }
  }

  max_power = 0;
  for (int i = DTMF_HALF_COUNT_NUM; i < DTMF_COUNT_NUM; i++)
  {
    if (power_all[i] > max_power)
    {
      max_power = power_all[i];
      col = i;
    }
  }

  if (power_all[row] == 0 && power_all[col] == 0)
  {
    new_dig = true;
  }

  if (power_all[row] > 800 && power_all[col] > 800 && new_dig)
  {
    char digit = row_col[row][col - DTMF_HALF_COUNT_NUM];

    LOG_DEBUG("digit: %c", digit);
    new_dig = false;
  }
}

