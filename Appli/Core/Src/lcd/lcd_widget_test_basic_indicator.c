#include "lcd_widget_test_basic_indicator.h"

void DrawStatusBox(uint8_t index, BasicIndicatorStatus *status)
{
    if (index >= STATUS_COUNT || !status) return;

    IndicatorList item = status->items[index];
    uint16_t baseX = status->baseX;
    uint16_t baseY = status->baseY;

    const char *text = item.label;
    uint16_t rectX = baseX + index * (STATUS_BOX_SIZE + STATUS_BOX_SPACING_X);
    uint16_t rectY = baseY + STATUS_BOX_SPACING_Y;
    uint16_t fillColor = item.status ? COLOR_GREEN : COLOR_RED;

	FontDef *font = &Font_7x10;
	uint16_t textLen = strlen(text) * font->width;
    uint16_t textX = rectX + (STATUS_BOX_SIZE - textLen)/2;
    uint16_t textY = rectY - STATUS_LABEL_OFFSET_Y;

    hx8357_write_string(textX, textY, text, font, COLOR_WHITE, COLOR_BLACK);
    hx8357_draw_rect(rectX, rectY, STATUS_BOX_SIZE, STATUS_BOX_SIZE, 1, COLOR_WHITE, fillColor);
}

void TestBasicDisplay_DrawAll(BasicIndicatorStatus *status)
{
	if(!status) return;

    const char *text = status->label;
	uint16_t baseX = status->baseX;
	uint16_t baseY = status->baseY;
	uint16_t lineWidth = (STATUS_BOX_SIZE * STATUS_COUNT) + (STATUS_BOX_SPACING_X * (STATUS_COUNT - 1));

	hx8357_fill_rect(baseX, baseY, lineWidth, 1, COLOR_WHITE);

	FontDef *font = &Font_11x18;
	uint16_t textWidth = font->width * strlen(text);
	uint16_t textX = baseX + (lineWidth - textWidth) / 2;
	uint16_t textY = baseY - font->height / 2;

	hx8357_write_string(textX, textY, text, font, COLOR_WHITE, COLOR_BLACK);

	for (uint8_t i = 0; i < STATUS_COUNT; ++i)
	{
		DrawStatusBox(i, status);
		//osDelay(3);
	}
}
