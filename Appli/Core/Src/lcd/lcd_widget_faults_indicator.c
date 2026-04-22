#include "lcd_widget_faults_indicator.h"
#include "lcd_widget_test_basic_indicator_list.h"

#define FAULTS_COUNT 8

static BasicIndicatorList statusFaults = {
	.baseX = 0,
	.baseY = 0,
	.items = {
		{"AC Voltage",      true},
		{"Battery Voltage", true},
		{"Charger Unit",    false},
		{"Amp",             false},
		{"Drv",             true},
		{"C&M Interface",   false},
		{"Door",            false},
		{"Flood",           true}
	}
};

void FaultsDisplay_DrawAll(uint16_t baseX, uint16_t baseY)
{
	statusFaults.baseX = baseX;
	statusFaults.baseY = baseY;

	for (uint8_t i = 0; i < FAULTS_COUNT; ++i)
	{
		DrawStatusIndicatorList(i, &statusFaults);
	}
}

void FaultsDisplay_SetStatus(uint8_t index, bool status)
{
    if (index < 0 || index >= FAULTS_COUNT) return;

    if (statusFaults.items[index].status != status)
    {
    	statusFaults.items[index].status = status;
    	DrawStatusIndicatorList(index, &statusFaults);
    }
}
