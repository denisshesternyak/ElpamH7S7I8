#include "lcd_widget_volume.h"
#include <string.h>
#include "defines.h"
#include "hx8357d.h"
#include "fonts.h"
#include <stdio.h>
#include "sdfs.h"

#define LOGGING_X			20
#define LOGGING_Y 			101

static FIL log_file;
static bool is_log_open = false;
static char buf[128];

static uint16_t check_log_lvl (const char *str)
{
  const char *p = &str[16];
  if (strcmp(p, "I") == 0)
    return COLOR_GREEN;
  else if (strcmp(p, "W") == 0)
      return COLOR_YELLOW;
  else if (strcmp(p, "E") == 0)
    return COLOR_RED;
  else if (strcmp(p, "F") == 0)
    return COLOR_MAGENTA;
  else
    return COLOR_WHITE;
}

void Logging_Init (void)
{
  if (!sdfs_is_file_exist_not_null(LOG_FILE_PATH_U))
      return;
  is_log_open = sdfs_open_file(&log_file, LOG_FILE_PATH_U);
}

void Logging_Draw (void)
{
  if(!is_log_open)
    return;

  FontDef *font = &Font_11x18;
  uint16_t pos_y = LOGGING_Y;

  while (sdfs_read_line(&log_file, (TCHAR *)buf, sizeof(buf)) > 0)
  {
    if(pos_y + font->height > hx8357_get_height())
      break;

    hx8357_write_alignedX_string(LOGGING_X, pos_y, buf, font, check_log_lvl(buf), COLOR_BLACK, ALIGN_LEFT);
    pos_y += font->height;
  }

  f_close(&log_file);
}
