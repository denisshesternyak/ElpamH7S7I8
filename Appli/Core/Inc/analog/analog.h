#ifndef INC_ANALOG_ANALOG_H_
#define INC_ANALOG_ANALOG_H_

#include <stdbool.h>
#include <stdint.h>

#define VDDA_APPLI              3300UL
#define ADC_BUFF_SIZE 		5

#define ADC_12V_DIV 		4619U
#define ADC_24V_DIV 		9218U
#define ADC_AMP_DIV 		1024U
#define ADC_DRV_DIV 		1024U

#define ADC_TASK_12V 		12000U
#define ADC_TASK_24V 		24000U
#define ADC_TASK_AMP 		1000U
#define ADC_TASK_DRV 		1000U

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
bool test_adc_value (ADC_channel n_ch, uint16_t value_adc);
bool check_voltage (ADC_channel n_ch, uint16_t vDiv, uint16_t task_mv, uint16_t delta_mv);
uint16_t getRData (uint16_t vDiv, uint16_t rawData);
uint16_t getRDataD (uint16_t vDiv, uint16_t rawData);

#endif /* INC_ANALOG_ANALOG_H_ */
