#ifndef INC_LCD_LCD_WIDGET_LOGGING_H_
#define INC_LCD_LCD_WIDGET_LOGGING_H_

#include <stdbool.h>
#include <stdint.h>

void Logging_Init(void);
void Logging_Close (void);
void Logging_ResetSeek (void);
void Logging_Draw (void);
void Logging_Increase (void);
void Logging_Decrease (void);

#endif /* INC_LCD_LCD_WIDGET_LOGGING_H_ */
