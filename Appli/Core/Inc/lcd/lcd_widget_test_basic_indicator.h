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

typedef enum {
  STATUS_IDLE,
  STATUS_OK,
  STATUS_BAD_M,
  STATUS_BAD_P,
  STATUS_BAD_1
} StatusTest_t;

typedef struct {
    const char* label;
    StatusTest_t status;
} Indicator;

typedef struct {
	uint16_t baseX;
	uint16_t baseY;
	const char* label;
	Indicator items[STATUS_COUNT];
}BasicIndicatorStatus;

const char* StatusToString (StatusTest_t status);
void DrawStatusBox(uint8_t index, BasicIndicatorStatus *status);
void TestBasicDisplay_DrawAll(BasicIndicatorStatus *status);

#endif /* INC_LCD_LCD_WIDGET_TEST_BASIC_INDICATOR_H_ */
