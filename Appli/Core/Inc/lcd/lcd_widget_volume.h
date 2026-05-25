#ifndef INC_LCD_LCD_WIDGET_VOLUME_H_
#define INC_LCD_LCD_WIDGET_VOLUME_H_

#include <stdbool.h>
#include <stdint.h>

void Volume_Init(void);
void Volume_IncreaseIndex (void);
void Volume_DecreaseIndex (void);
void Volume_ChangeNumber (uint8_t n);
void Volume_SendVolume (void);
void Volume_UpdateValue (void);

#endif /* INC_LCD_LCD_WIDGET_VOLUME_H_ */
