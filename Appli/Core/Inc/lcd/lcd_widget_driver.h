#ifndef INC_LCD_LCD_WIDGET_DRIVER_H_
#define INC_LCD_LCD_WIDGET_DRIVER_H_

#include <stdint.h>

void Init_Current_Driver (void);
uint8_t Get_Current_Driver(void);
void Set_Current_Driver (uint8_t value);
uint8_t get_min_ohm (void);
uint8_t get_mid_ohm (void);
uint8_t get_max_ohm (void);

#endif /* INC_LCD_LCD_WIDGET_DRIVER_H_ */
