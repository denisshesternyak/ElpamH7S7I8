#include "lcd_widget_faults_indicator.h"
#include "system_status.h"

#define FAULTS_COUNT 8

static BasicIndicatorList statusFaults = {
    .baseX = 0,
    .baseY = 0,
    .items = {
	{"AC Voltage",      STATUS_LIST_IDLE},
	{"Battery Voltage", STATUS_LIST_IDLE},
	{"Charger Unit",    STATUS_LIST_IDLE},
	{"Amp",             STATUS_LIST_IDLE},
	{"Drv",             STATUS_LIST_IDLE},
	{"C&M Interface",   STATUS_LIST_IDLE},
	{"Door",            STATUS_LIST_IDLE},
	{"Flood",           STATUS_LIST_IDLE}
    }
};

void FaultsDisplay_DrawAll (uint16_t baseX, uint16_t baseY)
{
  statusFaults.baseX = baseX;
  statusFaults.baseY = baseY;

  for (uint8_t i = 0; i < FAULTS_COUNT; ++i)
  {
    DrawStatusIndicatorList(i, &statusFaults);
  }
}

void FaultsDisplay_DrawOne (StatusIndName_t index, StatusIndTest_t status)
{
  if (index < 0 || index >= FAULTS_COUNT)
      return;

  if (statusFaults.items[index].status != status)
  {
    statusFaults.items[index].status = status;
    DrawStatusIndicatorList(index, &statusFaults);
  }
}

void FaultsDisplay_SetStatus (StatusIndName_t index, StatusIndTest_t status)
{
  if (index < 0 || index >= FAULTS_COUNT)
    return;

  if (statusFaults.items[index].status != status)
  {
    statusFaults.items[index].status = status;
  }
}
