#include "lcd_widget_volume.h"
#include <string.h>
#include "defines.h"
#include "hx8357d.h"
#include "fonts.h"
#include <stdio.h>
#include "sdfs.h"
#include "logger.h"

#define LOGGING_X			20
#define LOGGING_Y 			101

static FIL log_file;
static bool is_log_open = false;
static char buf[128];

static uint16_t logging_check_lvl (const char *str)
{
  const char *p = &str[16];

  if (*p == 'I')
    return COLOR_GREEN;
  else if (*p == 'W')
    return COLOR_YELLOW;
  else if (*p == 'E')
    return COLOR_RED;
  else if (*p == 'F')
    return COLOR_MAGENTA;
  else
    return COLOR_WHITE;
}

static void logging_remove_tag(char* str)
{
    char* bracket_start = strchr(str, '[');
    if (bracket_start == NULL) return;

    char* second_bracket = strchr(bracket_start + 1, '[');
    if (second_bracket == NULL) return;

    char* bracket_end = strchr(second_bracket, ']');
    if (bracket_end == NULL) return;

    char* content_start = bracket_end + 1;
    while (*content_start == ' ') {
        content_start++;
    }

    while (*content_start) {
        *second_bracket++ = *content_start++;
    }
    *second_bracket = '\0';
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
  const uint8_t w = font->width;
  const uint8_t h = font->height;
  const uint16_t max_char = (hx8357_get_width() - (LOGGING_X * 2)) / w;

  uint16_t pos_y = LOGGING_Y;

  while (1)
  {
    uint32_t len = sdfs_read_line(&log_file, buf);
    if(len == 0)
      break;

    buf[max_char] = '\0';

    uint16_t color = logging_check_lvl(buf);
//    LOG_DEBUG("%s, %d, %ld", buf, color, len);

    if(pos_y + font->height > hx8357_get_height())
      break;

    const char *str = (buf[0] == '\n') ? " " : buf;
    hx8357_write_alignedX_string(LOGGING_X, pos_y, str, font, color, COLOR_BLACK, ALIGN_LEFT);
    pos_y += h;
  }

  f_close(&log_file);
}
