#ifndef INC_LCD_LCD_WIDGET_REPORT_INDICATOR_H_
#define INC_LCD_LCD_WIDGET_REPORT_INDICATOR_H_

#include <stdint.h>
#include <stdbool.h>

void ReportIndicator_DrawAll(uint16_t baseX, uint16_t baseY);
void Report_setIndicator(uint8_t index, bool status);

#endif /* INC_LCD_LCD_WIDGET_REPORT_INDICATOR_H_ */
