#ifndef INC_LCD_LCD_WIDGET_DRV_PARAM_H_
#define INC_LCD_LCD_WIDGET_DRV_PARAM_H_

#include <stdbool.h>
#include <stdint.h>

void DrvParam_Init(void);
void DrvParam_IncreaseIndex (void);
void DrvParam_DecreaseIndex (void);
void DrvParam_ChangeNumber (uint8_t n);
void DrvParam_SendDrvParam (void);
void DrvParam_UpdateValue (void);

#endif /* INC_LCD_LCD_WIDGET_DRV_PARAM_H_ */
