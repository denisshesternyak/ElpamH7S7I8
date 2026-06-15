#ifndef INC_SCREENS_LCD_MENU_H_
#define INC_SCREENS_LCD_MENU_H_

#include "hx8357d.h"
#include "lcd_lang.h"
#include "lcd_strings_menu.h"
#include "lcd_types.h"
#include "stm32h7rsxx_hal.h"
#include "audio_types.h"

//#include "ds3231.h"
//#include "lcd_widget_volume_indicator.h"
#include <stdbool.h>
//#include "audiofs.h"
#include "logger.h"

#define MAX_MENU_ITEMS 		25

typedef struct
{
  const void *image;
  uint16_t x, y;
  uint16_t w, h;
} MenuImage;

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

typedef bool (*state_handler_t) (void);
typedef void (*volume_indicator) (uint8_t *, uint8_t *);

void lcd_volume_indicator (volume_indicator h);

void menu_init (void);
void menu_handle_button (KeyEvent_t event);
void update_date_time (void);
void update_progress_bar (uint8_t value);
void change_screen (MenuType type);

#endif /* INC_SCREENS_LCD_MENU_H_ */
