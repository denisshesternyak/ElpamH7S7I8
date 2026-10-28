#include "lcd_widget_test_drivers_indicator.h"

static const char *label = "Drivers";

static BasicIndicatorStatus statusDrivers = {
    .baseX = 0,
    .baseY = 0,
    .items = {
	{ "1", STATUS_IDLE },
	{ "2", STATUS_IDLE },
	{ "3", STATUS_IDLE },
	{ "4", STATUS_IDLE },
	{ "5", STATUS_IDLE },
	{ "6", STATUS_IDLE },
	{ "7", STATUS_IDLE },
	{ "8", STATUS_IDLE },
	{ "9", STATUS_IDLE },
	{ "10", STATUS_IDLE }
    }
};

void TestDrvDisplay_DrawAll (uint16_t baseX, uint16_t baseY)
{
  statusDrivers.baseX = baseX;
  statusDrivers.baseY = baseY;
  statusDrivers.label = label;

  for (uint8_t i = 0; i < STATUS_COUNT; ++i)
  {
    statusDrivers.items[i].status = STATUS_IDLE;
  }

  TestBasicDisplay_DrawAll(&statusDrivers);
}

void TestDrvDisplay_SetStatus (uint8_t index, StatusTest_t status)
{
  if (index >= STATUS_COUNT)
    return;

  if (statusDrivers.items[index].status != status)
  {
    statusDrivers.items[index].status = status;
    DrawStatusBox(index, &statusDrivers);
  }
}
