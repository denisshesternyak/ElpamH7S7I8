#include "lcd_widget_batteries_indicator.h"

#define BATTERIES_COUNT 3

static BasicIndicatorList statusBatteries = {
    .baseX = 0,
    .baseY = 0,
    .items = {
	{ "Mains", STATUS_LIST_IDLE },
	{ "Under Voltage", STATUS_LIST_IDLE },
	{ "Over Voltage", STATUS_LIST_IDLE }
    }
};

void BatteriesDisplay_DrawAll (uint16_t baseX, uint16_t baseY)
{
  statusBatteries.baseX = baseX;
  statusBatteries.baseY = baseY;

  for (uint8_t i = 0; i < BATTERIES_COUNT; ++i)
  {
    statusBatteries.items[i].status = STATUS_LIST_IDLE;
    DrawStatusIndicatorList(i, &statusBatteries);
  }
}

void BatteriesDisplay_SetStatus (uint8_t index, StatusIndTest_t status)
{
  if (index < 0 || index >= BATTERIES_COUNT)
    return;

  if (statusBatteries.items[index].status != status)
  {
    statusBatteries.items[index].status = status;
    DrawStatusIndicatorList(index, &statusBatteries);
  }
}

