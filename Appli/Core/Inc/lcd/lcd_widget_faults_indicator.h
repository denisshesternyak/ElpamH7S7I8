#ifndef INC_LCD_LCD_WIDGET_FAULTS_INDICATOR_H_
#define INC_LCD_LCD_WIDGET_FAULTS_INDICATOR_H_

#include <stdbool.h>
#include <stdint.h>
#include "lcd_widget_test_basic_indicator_list.h"

void FaultsDisplay_DrawAll(uint16_t baseX, uint16_t baseY);
void FaultsDisplay_DrawOne (StatusIndName_t index, StatusIndTest_t status);
void FaultsDisplay_SetStatus(StatusIndName_t index, StatusIndTest_t status);

#endif /* INC_LCD_LCD_WIDGET_FAULTS_INDICATOR_H_ */
