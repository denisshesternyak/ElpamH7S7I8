#ifndef INC_LCD_LCD_TYPES_H_
#define INC_LCD_LCD_TYPES_H_

#include "keyboard.h"

typedef enum
{
  LCD_EVENT_BTN,
  LCD_EVENT_PROGRESS,
  LCD_EVENT_RTC,
  LCD_EVENT_SCREEN,
  LCD_EVENT_AMP,
  LCD_EVENT_DRV,
  LCD_EVENT_BATT
} LCDEvent;

typedef enum
{
  MENU_TYPE_UNK,
  MENU_TYPE_LIST,
  MENU_TYPE_MAIN,
  MENU_TYPE_PASSWORD,
  MENU_TYPE_SIREN_INFO,
  //MENU_TYPE_SCREEN,
  MENU_TYPE_MESSAGE_PLAY,
  MENU_TYPE_IDLE,
  MENU_TYPE_ANNOUNCEMENT,
  MENU_TYPE_REPORT,
  MENU_TYPE_TEST_BAT,
  MENU_TYPE_TEST_AMP,
  MENU_TYPE_TEST_DRIV,
  MENU_TYPE_CLOCK,
  MENU_TYPE_VOLUME,
  MENU_TYPE_LOG,
  MENU_TYPE_MOTOROLA,
  MENU_TYPE_DTMF,
  MENU_TYPE_PREVIOUS
} MenuType;

typedef struct
{
  uint8_t status;
  uint8_t ind;
} TestMsg_t;

typedef struct
{
  LCDEvent event;
  union
  {
    KeyEvent_t btn;
    MenuType screen;
    TestMsg_t testMsg;
    uint32_t value;
  };
} LCDTaskEvent_t;

#endif /* INC_LCD_LCD_TYPES_H_ */
