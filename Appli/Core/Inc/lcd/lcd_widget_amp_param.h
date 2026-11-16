#ifndef INC_LCD_LCD_WIDGET_AMP_PARAM_H_
#define INC_LCD_LCD_WIDGET_AMP_PARAM_H_

#include <stdbool.h>
#include <stdint.h>

void AmpParam_Init(void);
void AmpParam_IncreaseIndex (void);
void AmpParam_DecreaseIndex (void);
void AmpParam_ChangeNumber (uint8_t n);
void AmpParam_SendAmpParam (void);
void AmpParam_UpdateValue (void);

#endif /* INC_LCD_LCD_WIDGET_AMP_PARAM_H_ */
