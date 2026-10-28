#include "lcd_widget_test_ampl_indicator.h"

static const char *label = "Amplifires";

static BasicIndicatorStatus statusAmplifires = {
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

void TestAmplDisplay_DrawAll (uint16_t baseX, uint16_t baseY)
{
  statusAmplifires.baseX = baseX;
  statusAmplifires.baseY = baseY;
  statusAmplifires.label = label;

  for (uint8_t i = 0; i < STATUS_COUNT; ++i)
  {
    statusAmplifires.items[i].status = STATUS_IDLE;
  }

  TestBasicDisplay_DrawAll(&statusAmplifires);
}

void TestAmplDisplay_SetStatus (uint8_t index, StatusTest_t status)
{
  if (index >= STATUS_COUNT)
    return;

  if (statusAmplifires.items[index].status != status)
  {
    statusAmplifires.items[index].status = status;
    DrawStatusBox(index, &statusAmplifires);
  }
}
