#ifndef INC_ANALOG_ANALOG_H_
#define INC_ANALOG_ANALOG_H_

#include <stdbool.h>
#include <stdint.h>

#define VDDA_APPLI              3300UL
#define ADC_BUFF_SIZE 		5

typedef enum
{
  ADC_12V,
  ADC_24V,
  ADC_CURRENT_MEAS,
  ADC_AMPLIFIER_MEAS,
  ADC_DRIVER_MEAS
} ADC_channel;

void analog_init (void);
bool cmp_with_delta (uint16_t v1, uint16_t v2, uint16_t delta);
uint16_t get_adc_value (ADC_channel n_ch);
bool test_adc_value (ADC_channel n_ch, uint16_t value_mv);
uint16_t getRData (uint16_t vDiv, uint16_t rawData);
uint16_t getRDataD (uint16_t vDiv, uint16_t rawData);

#endif /* INC_ANALOG_ANALOG_H_ */
