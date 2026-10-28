#ifndef INC_LCD_LCD_WIDGET_TEST_DRIVERS_INDICATOR_H_
#define INC_LCD_LCD_WIDGET_TEST_DRIVERS_INDICATOR_H_

#include <stdbool.h>
#include <stdint.h>
#include "lcd_widget_test_basic_indicator.h"

void TestDrvDisplay_DrawAll(uint16_t baseX, uint16_t baseY);
void TestDrvDisplay_SetStatus(uint8_t index, StatusTest_t status);

#endif /* INC_LCD_LCD_WIDGET_TEST_DRIVERS_INDICATOR_H_ */
