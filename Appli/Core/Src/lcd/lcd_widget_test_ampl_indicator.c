#include "lcd_widget_test_basic_indicator.h"
#include "lcd_widget_test_ampl_indicator.h"

static const char* label = "Amplifires";

static BasicIndicatorStatus statusAmplifires = {
	.baseX = 0,
	.baseY = 0,
	.items = {
		{"1", true},
		{"2", true},
		{"3", false},
		{"4", false},
		{"5", true},
		{"6", false},
		{"7", false},
		{"8", true},
		{"9", false},
		{"10", true}
	}
};

//static void DrawCenteredLabelWithLines(uint16_t baseX, uint16_t baseY, const char* text, uint16_t width)
//{
//    uint16_t textLen = strlen(text) * Font_7x10.width;
//    uint16_t lineWidth = (width - textLen - 10) / 2;
//
//    hx8357_outline_rect(baseX, baseY + 8, baseX + lineWidth, baseY + 8, 1, COLOR_WHITE);
//
//    hx8357_write_string(baseX + lineWidth + 5, baseY, text, &Font_7x10, COLOR_WHITE, COLOR_BLACK);
//
//    hx8357_outline_rect(baseX + lineWidth + 5 + textLen + 5, baseY + 5, baseX + width, baseY + 5, 1, COLOR_WHITE);
//}


void TestAmplDisplay_DrawAll(uint16_t baseX, uint16_t baseY)
{
	statusAmplifires.baseX = baseX;
	statusAmplifires.baseY = baseY;
	statusAmplifires.label = label;

	TestBasicDisplay_DrawAll(&statusAmplifires);
}

void TestAmplDisplay_SetStatus(uint8_t index, bool status)
{
    if (index >= STATUS_COUNT) return;

    if (statusAmplifires.items[index].status != status) {
    	statusAmplifires.items[index].status = status;
        DrawStatusBox(index, &statusAmplifires);
    }
}
