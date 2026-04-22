#include "lcd_widget_test_basic_indicator.h"
#include "lcd_widget_test_drivers_indicator.h"

static const char* label = "Drivers";

static BasicIndicatorStatus statusDrivers = {
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

void TestDrvDisplay_DrawAll(uint16_t baseX, uint16_t baseY)
{
	statusDrivers.baseX = baseX;
	statusDrivers.baseY = baseY;
	statusDrivers.label = label;

	TestBasicDisplay_DrawAll(&statusDrivers);
}

void TestDrvDisplay_SetStatus(uint8_t index, bool status)
{
    if (index >= STATUS_COUNT) return;

    if (statusDrivers.items[index].status != status) {
    	statusDrivers.items[index].status = status;
        DrawStatusBox(index, &statusDrivers);
    }
}
