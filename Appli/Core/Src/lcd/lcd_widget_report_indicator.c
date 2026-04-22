#include "lcd_widget_report_indicator.h"
#include "hx8357d.h"
#include <string.h>

#define BOX_SIZE       32
#define BOX_SPACING_X  13
#define BOX_SPACING_Y  40	
#define TEXT_OFFSET_Y  10
#define TEXT_OFFSET_LABEL_Y  2

#define REPORT_INDICATOR_MAX 22
#define REPORT_ITEMS_PER_ROW_MAX 10

typedef struct {
	uint8_t index;
	const char* id;
    const char* label;   
    const char* symbol;  
    bool status;         
} ReportIndicator;

typedef struct {
	uint16_t baseX;
	uint16_t baseY;
	ReportIndicator items[REPORT_INDICATOR_MAX];
}ReportIndicatorStatus;

static ReportIndicatorStatus statusReportIndicators = {
	.baseX = 0,
	.baseY = 0,
	.items = {
		{0, "1",  "Oper", "H", true},
		{1, NULL,  NULL, NULL, false},
		{10, "3",  "Amp1/Drv1", NULL, true},
		{11, "4",  "Amp2/Drv2", NULL, true},
		{12, "5",  "Amp3/Drv3", NULL, true},
		{13, "6",  "Amp4/Drv4", NULL, true},
		{14, "7",  "Amp5/Drv5", NULL, true},
		{15, "8",  "Amp6/Drv6", NULL, true},
		{16, "9",  "Amp7/Drv7", NULL, true},
		{17, "10", "Amp8/Drv8", NULL, false},
		{18, "11", "Amp9/Drv9", NULL, true},
		{19, "12", "Amp10/Drv10",NULL,true},
		{20, NULL,  NULL, NULL, false},
		{21, "14", "Vol",   NULL, true},
		{22, "15", "Tone",   NULL, true},
		{23, "16", "P.S.",   NULL, true},
		{24, "17", "Oper",  NULL, false},
		{25, "18", "Batt",   NULL, true},
		{26, "19", "Charg", NULL, true},
		{27, "20", " AC",     NULL, true},
		{28, "21", "Flood",  NULL, true},
		{29, "22", "Door",   NULL, true}
	}
};

static void DrawOneIndicator(uint8_t index, ReportIndicatorStatus *status)
{
    if (index >= REPORT_INDICATOR_MAX || !status) return;

    ReportIndicator item = status->items[index];

    if (!item.id) return;

    const uint16_t color_text = COLOR_WHITE;
    const uint16_t color_bg = COLOR_BLACK;

    uint8_t row = item.index / REPORT_ITEMS_PER_ROW_MAX;
    uint8_t col = item.index % REPORT_ITEMS_PER_ROW_MAX;

    uint16_t x = status->baseX + col * (BOX_SIZE + BOX_SPACING_X);
    uint16_t y = status->baseY + row * (BOX_SIZE + BOX_SPACING_Y);

    uint16_t fillColor = item.status ? COLOR_GREEN : COLOR_RED;
    hx8357_draw_rect(x, y, BOX_SIZE, BOX_SIZE, 1, COLOR_WHITE, fillColor);

    FontDef *font = &Font_7x10;
	uint8_t idLen = strlen(item.id);
	uint16_t idX = x + (BOX_SIZE - (idLen * font->width)) / 2;
	uint16_t idY = y - TEXT_OFFSET_Y;
	hx8357_write_string(idX, idY, item.id, font, color_text, color_bg);

	if(item.symbol)
	{
		uint8_t len = strlen(item.symbol);
		uint16_t symX = x + (BOX_SIZE - (len * font->width)) / 2;
		uint16_t symY = y + (BOX_SIZE - font->height) / 2;
		hx8357_write_string(symX, symY, item.symbol, font, color_text, color_bg);
	}

	if(item.label)
	{
		const char* slash = strchr(item.label, '/');

		size_t len = strlen(item.label);
		uint16_t lableX = 0;
		uint16_t lableY = y + BOX_SIZE + TEXT_OFFSET_LABEL_Y;

		if (slash)
		{
			size_t len_slash = strlen(slash);
			size_t len_part = len - len_slash;
			lableX = x + (BOX_SIZE - (len_part * font->width)) / 2;
			hx8357_writeN_string(lableX, lableY, item.label, len_part, font, color_text, color_bg);

			lableX = x + (BOX_SIZE - ((len_slash-1) * font->width)) / 2;
			hx8357_write_string(lableX, lableY + font->height, slash + 1, font, color_text, color_bg);
		}
		else
		{
			lableX = x + (BOX_SIZE - (len * font->width)) / 2;
			hx8357_write_string(lableX, lableY, item.label, font, color_text, color_bg);
		}
	}
}

void ReportIndicator_DrawAll(uint16_t baseX, uint16_t baseY)
{
	statusReportIndicators.baseX = baseX;
	statusReportIndicators.baseY = baseY;

    for (uint8_t i = 0; i < REPORT_INDICATOR_MAX; ++i) {
        DrawOneIndicator(i, &statusReportIndicators);
        //osDelay(1);
    }
}

void Report_setIndicator(uint8_t index, bool status)
{
    if (index >= REPORT_INDICATOR_MAX) return;

    if (statusReportIndicators.items[index].status != status) {
    	statusReportIndicators.items[index].status = status;
        DrawOneIndicator(index, &statusReportIndicators);
    }
}
