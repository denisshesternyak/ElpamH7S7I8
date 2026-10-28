#ifndef INC_LCD_LCD_WIDGET_BATTERIES_INDICATOR_H_
#define INC_LCD_LCD_WIDGET_BATTERIES_INDICATOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "lcd_widget_test_basic_indicator_list.h"

void BatteriesDisplay_DrawAll(uint16_t baseX, uint16_t baseY);
void BatteriesDisplay_SetStatus(uint8_t index, StatusIndTest_t status);

#endif /* INC_LCD_LCD_WIDGET_BATTERIES_INDICATOR_H_ */
