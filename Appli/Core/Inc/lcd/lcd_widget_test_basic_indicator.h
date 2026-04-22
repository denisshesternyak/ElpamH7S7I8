#ifndef INC_LCD_LCD_WIDGET_TEST_BASIC_INDICATOR_H_
#define INC_LCD_LCD_WIDGET_TEST_BASIC_INDICATOR_H_

#include "hx8357d.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define STATUS_COUNT              10
#define STATUS_BOX_SIZE           32
#define STATUS_BOX_SPACING_X      13
#define STATUS_BOX_SPACING_Y      35
#define STATUS_LABEL_OFFSET_Y     15

typedef struct {
    const char* label;
    bool status;
} IndicatorList;

typedef struct {
	uint16_t baseX;
	uint16_t baseY;
	const char* label;
	IndicatorList items[STATUS_COUNT];
}BasicIndicatorStatus;

void DrawStatusBox(uint8_t index, BasicIndicatorStatus *status);
void TestBasicDisplay_DrawAll(BasicIndicatorStatus *status);

#endif /* INC_LCD_LCD_WIDGET_TEST_BASIC_INDICATOR_H_ */
