#ifndef INC_LCD_LCD_WIDGET_TEST_BASIC_INDICATOR_LIST_H_
#define INC_LCD_LCD_WIDGET_TEST_BASIC_INDICATOR_LIST_H_

#include "hx8357d.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define STATUS_MAX_INDICATOR        10
#define STATUS_INDICATOR_SIZE       15
#define STATUS_INDICATOR_RIGHT_X    230

#define INDICATOR_START_X           0
#define INDICATOR_START_Y           0
#define INDICATOR_SPACING_Y         25

typedef enum
{
  STATUS_LIST_IDLE,
  STATUS_LIST_OK,
  STATUS_LIST_BAD
} StatusIndTest_t;

typedef enum
{
  STATUS_LIST_AC_VOLTAGE,
  STATUS_LIST_BATTERY,
  STATUS_LIST_CHARGER,
  STATUS_LIST_AMP,
  STATUS_LIST_DRV,
  STATUS_LIST_CM_INTERFACE,
  STATUS_LIST_DOOR,
  STATUS_LIST_FLOOD
} StatusIndName_t;

typedef struct
{
  const char *label;
  StatusIndTest_t status;
} IndicatorList;

typedef struct
{
  uint16_t baseX;
  uint16_t baseY;
  IndicatorList items[STATUS_MAX_INDICATOR];
} BasicIndicatorList;

void DrawStatusIndicatorList (int index, BasicIndicatorList *list);

#endif /* INC_LCD_LCD_WIDGET_TEST_BASIC_INDICATOR_LIST_H_ */
