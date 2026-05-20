#ifndef INC_LCD_LCD_WIDGET_PASSWORD_H_
#define INC_LCD_LCD_WIDGET_PASSWORD_H_

#include <stdbool.h>

bool Password_IsCorrect(void); 
void Draw_MENU_TYPE_PASSWORD(void);
void Password_AddChar(char c);
void Password_Reset(bool redraw);
void Password_Enter(void);
void Password_Backspace(void);


#endif /* INC_LCD_LCD_WIDGET_PASSWORD_H_ */
