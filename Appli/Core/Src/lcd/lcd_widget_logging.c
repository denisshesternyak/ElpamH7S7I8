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
#define LOGGING_Y2 			125
#define LOGGING_MAX_LVL 		4

static FIL log_file;
static bool is_log_open;
static char buf[128];
static int8_t cur_lvl;

static int8_t logging_get_lvl (const char *str)
{
  const char *p = &str[16];

  if (*p == 'I')
    return 0;
  else if (*p == 'W')
    return 1;
  else if (*p == 'E')
    return 2;
  else if (*p == 'F')
    return 3;
  else
    return 0;
}

static uint16_t logging_get_color (const uint8_t lvl)
{
  switch(lvl)
  {
    case 0:
      return COLOR_GREEN;
      break;
    case 1:
      return COLOR_YELLOW;
      break;
    case 2:
      return COLOR_RED;
      break;
    case 3:
      return COLOR_MAGENTA;
      break;
    default:
      return COLOR_WHITE;
  }
}


static void logging_remove_tag (char *str)
{
  char *bracket_start = strchr(str, '[');
  if (bracket_start == NULL)
    return;

  char *second_bracket = strchr(bracket_start + 1, '[');
  if (second_bracket == NULL)
    return;

  char *bracket_end = strchr(second_bracket, ']');
  if (bracket_end == NULL)
    return;

  char *content_start = bracket_end + 1;
  while (*content_start == ' ')
  {
    content_start++;
  }

  while (*content_start)
  {
    *second_bracket++ = *content_start++;
  }
  *second_bracket = '\0';
}

void Logging_Init (void)
{
  if (!sdfs_is_file_exist_not_null(LOG_FILE_PATH_U))
    return;
  is_log_open = sdfs_open_file(&log_file, LOG_FILE_PATH_U);
  cur_lvl = 0;
}

void Logging_ResetSeek (void)
{
  f_lseek(&log_file, 0);
}

void Logging_Close (void)
{
  f_close(&log_file);
  is_log_open = false;
}

static void logging_draw_lvl (void)
{
  FontDef *font = &Font_11x18;

  hx8357_write_alignedX_string(LOGGING_X, LOGGING_Y, "[INFO]", font, COLOR_GREEN,
      (cur_lvl == 0) ? COLOR_BLUE : COLOR_BLACK, ALIGN_LEFT);
  hx8357_write_alignedX_string(LOGGING_X + 115, LOGGING_Y, "[WARN]", font, COLOR_YELLOW,
      (cur_lvl == 1) ? COLOR_BLUE : COLOR_BLACK, ALIGN_LEFT);
  hx8357_write_alignedX_string(LOGGING_X + 230, LOGGING_Y, "[ERROR]", font, COLOR_RED,
      (cur_lvl == 2) ? COLOR_BLUE : COLOR_BLACK, ALIGN_LEFT);
  hx8357_write_alignedX_string(LOGGING_X, LOGGING_Y, "[FATAL]", font, COLOR_MAGENTA,
      (cur_lvl == 3) ? COLOR_BLUE : COLOR_BLACK, ALIGN_RIGHT);
}

void Logging_Draw (void)
{
  if (!is_log_open)
    return;

  FontDef *font = &Font_11x18;
  const uint8_t w = font->width;
  const uint8_t h = font->height;
  const uint16_t max_char = (hx8357_get_width() - (LOGGING_X * 2)) / w;

  logging_draw_lvl();

  uint16_t pos_y = LOGGING_Y2;

  while (sdfs_read_line(&log_file, buf) > 0)
  {
    if (pos_y + font->height > hx8357_get_height())
      break;

    const int8_t lvl = logging_get_lvl(buf);
    if(lvl != cur_lvl)
      continue;

    uint16_t color = COLOR_BLACK;
    const char *str = buf;
    if (buf[0] == '\n')
    {
      str = " ";
    }
    else
    {
      color = logging_get_color(lvl);
      logging_remove_tag(buf);
      buf[max_char] = '\0';
    }

//    LOG_DEBUG("%s, %d, %d", buf, color, lvl);

    hx8357_write_alignedX_string(LOGGING_X, pos_y, str, font, color, COLOR_BLACK, ALIGN_LEFT);
    pos_y += h;
  }
}

void Logging_Increase (void)
{
  cur_lvl++;
  if (cur_lvl > LOGGING_MAX_LVL-1)
    cur_lvl = 0;
}

void Logging_Decrease (void)
{
  cur_lvl--;
  if (cur_lvl < 0)
    cur_lvl = LOGGING_MAX_LVL-1;
}
