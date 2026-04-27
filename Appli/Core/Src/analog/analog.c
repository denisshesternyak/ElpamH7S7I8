#include "analog.h"
#include "stm32h7rsxx_hal.h"
#include "adc.h"

static uint16_t adc_buff[ADC_BUFF_SIZE];

void analog_init (void)
{
  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buff, ADC_BUFF_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
}

bool cmp_with_delta (uint16_t v1, uint16_t v2, uint16_t delta)
{
  uint16_t dif = (v1 > v2) ? (v1 - v2) : (v2 - v1);
  return (dif < delta);
}

uint16_t get_adc_value (ADC_channel n_ch)
{
  return adc_buff[n_ch];
}

bool test_adc_value (ADC_channel n_ch, uint16_t value_adc)
{
  uint16_t val_adc = get_adc_value(n_ch);
  return cmp_with_delta(val_adc, value_adc, value_adc / 20);
}

bool check_voltage (ADC_channel n_ch, uint16_t vDiv, uint16_t task_mv, uint16_t delta_mv)
{
  uint16_t val_adc = get_adc_value(n_ch);
  uint16_t val_mv = getRDataD(vDiv, val_adc);
  return cmp_with_delta(task_mv, val_mv, delta_mv);
}

uint16_t getRData (uint16_t vDiv, uint16_t rawData)
{
  return vDiv * __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, rawData, LL_ADC_RESOLUTION_12B);
}

uint16_t getRDataD (uint16_t vDiv, uint16_t rawData)
{
  uint32_t val = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, rawData, LL_ADC_RESOLUTION_12B);
  return (uint16_t) ((vDiv * val) / 1024);
}

