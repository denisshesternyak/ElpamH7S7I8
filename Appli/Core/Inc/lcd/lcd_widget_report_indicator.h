#ifndef INC_LCD_LCD_WIDGET_REPORT_INDICATOR_H_
#define INC_LCD_LCD_WIDGET_REPORT_INDICATOR_H_

#include <stdint.h>
#include <stdbool.h>

#define REPORT_INDICATOR_MAX 22
#define REPORT_ITEMS_PER_ROW_MAX 10

typedef enum {
  REPORT_LIST_OPER,
  REPORT_LIST_NULL1,
  REPORT_LIST_AMP_DRV_1,
  REPORT_LIST_AMP_DRV_2,
  REPORT_LIST_AMP_DRV_3,
  REPORT_LIST_AMP_DRV_4,
  REPORT_LIST_AMP_DRV_5,
  REPORT_LIST_AMP_DRV_6,
  REPORT_LIST_AMP_DRV_7,
  REPORT_LIST_AMP_DRV_8,
  REPORT_LIST_AMP_DRV_9,
  REPORT_LIST_AMP_DRV_10,
  REPORT_LIST_NULL2,
  REPORT_LIST_VOL,
  REPORT_LIST_TON,
  REPORT_LIST_PS,
  REPORT_LIST_OPER2,
  REPORT_LIST_BATT,
  REPORT_LIST_CHARG,
  REPORT_LIST_AC,
  REPORT_LIST_FLOOD,
  REPORT_LIST_DOOR
} ReportIndName_t;

typedef struct
{
  uint8_t index;
  const char *id;
  const char *label;
  const char *symbol;
  bool status;
} ReportIndicator;

typedef struct
{
  uint16_t baseX;
  uint16_t baseY;
  ReportIndicator items[REPORT_INDICATOR_MAX];
} ReportIndicatorStatus;

void ReportIndicator_DrawAll(uint16_t baseX, uint16_t baseY);
void ReportIndicator_DrawOne (ReportIndName_t index, bool status);
void Report_SetStatus(ReportIndName_t index, bool status);

#endif /* INC_LCD_LCD_WIDGET_REPORT_INDICATOR_H_ */
