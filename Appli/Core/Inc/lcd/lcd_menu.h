#ifndef INC_SCREENS_LCD_MENU_H_
#define INC_SCREENS_LCD_MENU_H_

#include "hx8357d.h"
#include "keyboard.h"
#include "lcd_lang.h"
#include "lcd_strings_menu.h"
#include "stm32h7rsxx_hal.h"

//#include "ds3231.h"
//#include "lcd_widget_volume_indicator.h"
#include <stdbool.h>
//#include "audiofs.h"
#include "logger.h"

#define INACTIVITY_TIMEOUT_MS 	60
#define BACKLIGHT_TIMEOUT_MS 	10

#define MAX_MENU_ITEMS 		25

typedef struct
{
  const void *image;
  uint16_t x, y;
  uint16_t w, h;
} MenuImage;

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
  MENU_TYPE_PREVIOUS
} MenuType;

typedef enum
{
  LCD_EVENT_BTN,
  LCD_EVENT_PROGRESS,
  LCD_EVENT_RTC,
  LCD_EVENT_SCREEN
} LCDEvent;

typedef struct
{
  LCDEvent event;
  KeyEvent_t btn;
  MenuType screen;
  uint32_t value;
} LCDTaskEvent_t;

typedef void (*MenuButtonHandler) (KeyEvent_t event);
typedef void (*MenuAction) (void);

typedef struct MenuItem
{
  const char *name[LANG_COUNT];
  MenuAction prepareAction;
  MenuAction postAction;
  struct Menu *submenu;
} MenuItem;

typedef struct Menu
{
  struct Menu *parent;
  MenuType type;
  uint8_t scrollOffset;
  uint8_t oldOffset;
  MenuItem items[MAX_MENU_ITEMS];
  uint8_t itemCount;
  uint8_t currentSelection;
  uint8_t oldSelection;
  const char *screenText[LANG_COUNT];
  const char *textFilename;
  MenuImage *imageData;

  MenuButtonHandler buttonHandler;
} Menu;

void menu_init (void);
void menu_handle_button (KeyEvent_t event);
void update_date_time (void);
void update_progress_bar (uint8_t value);
void change_screen (MenuType type);

#endif /* INC_SCREENS_LCD_MENU_H_ */
