#include "tester.h"
#include "analog.h"
#include "cmsis_os2.h"

static void tester_amp_turn_on (tester_signals_t n)
{
  switch(n)
  {
    case SYSTEM_AMP_T1_ACT: AMP_T1_ACT_ON; break;
    case SYSTEM_AMP_T2_ACT: AMP_T2_ACT_ON; break;
    case SYSTEM_AMP_T3_ACT: AMP_T3_ACT_ON; break;
    case SYSTEM_AMP_T4_ACT: AMP_T4_ACT_ON; break;
    case SYSTEM_AMP_T5_ACT: AMP_T5_ACT_ON; break;
    case SYSTEM_AMP_T6_ACT: AMP_T6_ACT_ON; break;
    case SYSTEM_AMP_T7_ACT: AMP_T7_ACT_ON; break;
    case SYSTEM_AMP_T8_ACT: AMP_T8_ACT_ON; break;
    case SYSTEM_AMP_T9_ACT: AMP_T9_ACT_ON; break;
    case SYSTEM_AMP_T10_ACT: AMP_T10_ACT_ON; break;
    default: break;
  }
}

static void tester_amp_turn_off (tester_signals_t n)
{
  switch(n)
  {
    case SYSTEM_AMP_T1_ACT: AMP_T1_ACT_OFF; break;
    case SYSTEM_AMP_T2_ACT: AMP_T2_ACT_OFF; break;
    case SYSTEM_AMP_T3_ACT: AMP_T3_ACT_OFF; break;
    case SYSTEM_AMP_T4_ACT: AMP_T4_ACT_OFF; break;
    case SYSTEM_AMP_T5_ACT: AMP_T5_ACT_OFF; break;
    case SYSTEM_AMP_T6_ACT: AMP_T6_ACT_OFF; break;
    case SYSTEM_AMP_T7_ACT: AMP_T7_ACT_OFF; break;
    case SYSTEM_AMP_T8_ACT: AMP_T8_ACT_OFF; break;
    case SYSTEM_AMP_T9_ACT: AMP_T9_ACT_OFF; break;
    case SYSTEM_AMP_T10_ACT: AMP_T10_ACT_OFF; break;
    default: break;
  }
}

void tester_all_on (void)
{
  AMP_ON_ACT_ON;
  DRV_ON_ACT_ON;

  for(uint8_t i = 0; i < AMP_COUNT; i++)
  {
    tester_amp_turn_on((tester_signals_t)i);
  }
}

void tester_all_off (void)
{
  AMP_ON_ACT_OFF;
  DRV_ON_ACT_OFF;

  for(uint8_t i = 0; i < AMP_COUNT; i++)
  {
    tester_amp_turn_off((tester_signals_t)i);
  }
}

bool tester_check_amplifier (tester_signals_t n)
{
  tester_amp_turn_on(n);

  AMP_ON_ACT_ON;
  osDelay(1);

  bool is_amp = check_voltage(ADC_AMPLIFIER_MEAS, ADC_AMP_DIV, ADC_TASK_AMP, 100);

  AMP_ON_ACT_OFF;

  tester_amp_turn_off(n);

  return is_amp;
}

bool tester_check_driver (tester_signals_t n)
{
  tester_amp_turn_on(n);

  DRV_ON_ACT_ON;
  osDelay(1);

  bool is_drv = check_voltage(ADC_DRIVER_MEAS, ADC_DRV_DIV, ADC_TASK_DRV, 100);

  DRV_ON_ACT_OFF;

  tester_amp_turn_off(n);

  return is_drv;
}
