#include "lcd_widget_batteries_indicator.h"
#include "lcd_widget_test_basic_indicator_list.h"

#define BATTERIES_COUNT 3

static BasicIndicatorList statusBatteries = {
	.baseX = 0,
	.baseY = 0,
    .items = {
		{"Mains",           true},
		{"Under Voltage",   true},
		{"Over Voltage",    false}
    }
};
void BatteriesDisplay_DrawAll(uint16_t baseX, uint16_t baseY)
{
	statusBatteries.baseX = baseX;
	statusBatteries.baseY = baseY;

    for (uint8_t i = 0; i < BATTERIES_COUNT; ++i)
    {
    	DrawStatusIndicatorList(i, &statusBatteries);
    }
}

void BatteriesDisplay_SetStatus(uint8_t index, bool status)
{
    if (index < 0 || index >= BATTERIES_COUNT) return;

    if (statusBatteries.items[index].status != status)
    {
    	statusBatteries.items[index].status = status;
    	DrawStatusIndicatorList(index, &statusBatteries);
    }
}


