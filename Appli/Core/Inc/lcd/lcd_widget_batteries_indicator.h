#ifndef INC_LCD_LCD_WIDGET_BATTERIES_INDICATOR_H_
#define INC_LCD_LCD_WIDGET_BATTERIES_INDICATOR_H_

#include <stdint.h>
#include <stdbool.h>

void BatteriesDisplay_DrawAll(uint16_t baseX, uint16_t baseY);
void BatteriesDisplay_SetStatus(uint8_t index, bool status);

#endif /* INC_LCD_LCD_WIDGET_BATTERIES_INDICATOR_H_ */
