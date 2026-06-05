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
#define MAX_LOG_LINES   		11

#if !USED_SD
static FIL log_file;
static bool is_log_open;
#endif

typedef struct
{
  uint16_t log_line_count;
  uint16_t log_line_index;
  char log_lines[MAX_LOG_LINES][FF_MAX_LFN];
}FilteredLogs_t ;

static FilteredLogs_t filters[LOGGING_MAX_LVL];

static char buf[FF_MAX_LFN];
static char temp_line[FF_MAX_LFN];
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
    return -1;
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

static void logger_get_lines (void)
{
  while (1)
  {
#if !USED_SD
    uint32_t len = sdfs_read_line(&log_file, buf);
#else
    uint32_t len = sdfs_read_line(&logger.log_file, buf);
#endif
    if (!len)
      break;

    if (len == 1)
      continue;

    int8_t lvl = logging_get_lvl(buf);
    if (lvl < 0 || lvl >= LOGGING_MAX_LVL)
      continue;

    FilteredLogs_t *filter = &filters[lvl];

    strncpy(filter->log_lines[filter->log_line_index], buf, FF_MAX_LFN-1);
    filter->log_lines[filter->log_line_index][FF_MAX_LFN-1] = '\0';

    filter->log_line_index = (filter->log_line_index + 1) % MAX_LOG_LINES;
    if (filter->log_line_count < MAX_LOG_LINES)
      filter->log_line_count++;
  }
}

void Logging_Init (void)
{
  if (!sdfs_is_file_exist_not_null(LOG_FILE_PATH_U))
    return;

#if !USED_SD
  is_log_open = sdfs_open_file(&log_file, LOG_FILE_PATH_U);
#else
  if(logger.is_log_open)
  {
    f_lseek(&logger.log_file, 0);
  }
#endif
  cur_lvl = 0;

  memset(&filters, 0, sizeof(filters));
  logger_get_lines();
}

void Logging_ResetSeek (void)
{
#if !USED_SD
  f_lseek(&log_file, 0);
#else
  f_lseek(&logger.log_file, 0);
#endif
}

void Logging_Close (void)
{
#if !USED_SD
  f_close(&log_file);
  is_log_open = false;
#else
  if(logger.is_log_open)
  {
    f_lseek(&logger.log_file, f_size(&logger.log_file));
  }
#endif
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
#if !USED_SD
  if (!is_log_open)
   return;
#else
  if (!logger.is_log_open)
    return;
#endif

  FontDef *font = &Font_11x18;
  const uint8_t w = font->width;
  const uint8_t h = font->height;
  const uint16_t max_char = (hx8357_get_width() - (LOGGING_X * 2)) / w;

  logging_draw_lvl();

  uint16_t pos_y = LOGGING_Y2;

  FilteredLogs_t *filter = &filters[cur_lvl];
  if(!filter)
    return;

  char temp_buf[max_char + 1];
  uint16_t color = logging_get_color(cur_lvl);

  for (uint16_t i = 0; i < filter->log_line_count; i++)
  {
    if (pos_y + h > hx8357_get_height())
	break;

    uint16_t idx = (filter->log_line_index - 1 - i + MAX_LOG_LINES) % MAX_LOG_LINES;
    char *line = filter->log_lines[idx];

    strcpy(temp_line, line);
    logging_remove_tag(temp_line);

    uint32_t len = strlen(temp_line);
    char *str = (len == 0) ? " " : temp_line;

    uint32_t remaining = len ? len : 1;

    while (remaining > 0)
    {
      uint32_t chunk = (remaining > max_char) ? max_char : remaining;
      strncpy(temp_buf, str, chunk);
      temp_buf[chunk] = '\0';

      hx8357_write_alignedX_string(LOGGING_X, pos_y, temp_buf, font, color, COLOR_BLACK, ALIGN_LEFT);

      pos_y += h;
      if (pos_y + h > hx8357_get_height())
	  return;

      str += chunk;
      remaining -= chunk;
    }
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
