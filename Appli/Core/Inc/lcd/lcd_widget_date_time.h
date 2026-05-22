#ifndef INC_LCD_LCD_WIDGET_DATETIME_H_
#define INC_LCD_LCD_WIDGET_DATETIME_H_

#include <stdbool.h>
#include <stdint.h>

void DateTime_IncreaseIndex (void);
void DateTime_DecreaseIndex (void);
void DateTime_ChangeNumber (uint8_t n);
void DateTime_Prepare (void);
void DateTime_Set (void);
void DateTime_HeaderClock(void);
void Draw_MENU_TYPE_CLOCK(void);

#endif /* INC_LCD_LCD_WIDGET_DATETIME_H_ */
