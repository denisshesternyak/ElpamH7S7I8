#include "lcd_widget_test_basic_indicator_list.h"

void DrawStatusIndicatorList(int index, BasicIndicatorList *list)
{
    if (!list) return;

    IndicatorList item = list->items[index];

    uint16_t baseX = list->baseX;
    uint16_t baseY = list->baseY;

    uint16_t textX = baseX + INDICATOR_START_X;
    uint16_t textY = baseY + INDICATOR_START_Y + index * INDICATOR_SPACING_Y;

    uint16_t rectX = baseX + STATUS_INDICATOR_RIGHT_X - STATUS_INDICATOR_SIZE;
    uint16_t rectY = textY + 2;

    uint16_t fillColor = COLOR_DARKGRAY;

    switch(item.status)
    {
      case STATUS_LIST_IDLE:
	break;
      case STATUS_LIST_OK:
	fillColor = COLOR_GREEN;
	break;
      case STATUS_LIST_BAD:
	fillColor = COLOR_RED;
	break;
      default:
	break;
    }

    hx8357_write_string(textX, textY, item.label, &Font_11x18, COLOR_WHITE, COLOR_BLACK);
    hx8357_draw_rect(rectX, rectY, STATUS_INDICATOR_SIZE, STATUS_INDICATOR_SIZE, 1, COLOR_WHITE, fillColor);
}
