#include "lcd_widget_date_time.h"
#include <string.h>
#include "defines.h"
#include "hx8357d.h"
#include "fonts.h"
#include <stdio.h>
#include <stdbool.h>
#include "rtc.h"

#define DATETIME_HANDLER		&hrtc

#define DATETIME_TEXT_LINE1_Y 		190

#define MAX_UPDATE_STR			"UPDATED"
#define MAX_UPDATE_STR_EMPTY 		"       "

#define CLOCK_MAX_SYMBOLS		12

#define TIME_X_POS 			104
#define TIME_Y_POS 			153

#define TIME_HEADER_Y_POS 		5

static RTC_TimeTypeDef sTime = { 0 };
static RTC_DateTypeDef sDate = { 0 };
static int8_t current_symbol;

static char datetimeBuffer[CLOCK_MAX_SYMBOLS] = { 0 };

static void DateTime_change_status (const char *str)
{
  FontDef *font = &Font_11x18;
  hx8357_write_alignedX_string(0, DATETIME_TEXT_LINE1_Y, str, font, COLOR_WHITE, COLOR_BLACK, ALIGN_CENTER);
}

static bool DateTime_CheckSymbol (void)
{
  uint8_t field_index = current_symbol / 2;
  uint8_t index = field_index * 2;
  uint8_t data = (datetimeBuffer[index] * 10) + datetimeBuffer[index + 1];

  uint8_t range[] = {31,12,99,23,59,59};
  uint8_t* fields[] = {&sDate.Date, &sDate.Month, &sDate.Year,
                     &sTime.Hours, &sTime.Minutes, &sTime.Seconds};

  uint8_t val = range[field_index];
  bool overflow = data > val;

  if(overflow)
  {
    datetimeBuffer[index] = val / 10;
    datetimeBuffer[index+1] = val % 10;
    *fields[field_index] = val;
  }
  else
  {
    *fields[field_index] = data;
  }

  return overflow;
}

static void DateTime_CharUpdate(uint16_t field_bg)
{
  FontDef *font = &Font_16x26;
  const uint16_t pos_in_str[12] = {0,1, 3,4, 6,7, 10,11, 13,14, 16,17};

  uint8_t field_index = current_symbol / 2;
  uint8_t digit_pos = current_symbol % 2;

  uint8_t value = 0;
  switch (field_index)
  {
    case 0:
      value = sDate.Date;
      break;
    case 1:
      value = sDate.Month;
      break;
    case 2:
      value = sDate.Year;
      break;
    case 3:
      value = sTime.Hours;
      break;
    case 4:
      value = sTime.Minutes;
      break;
    case 5:
      value = sTime.Seconds;
      break;
  }

  uint8_t digit = (digit_pos == 0) ? (value / 10) : (value % 10);

  char str[2] = { '0' + digit, '\0' };

  uint16_t x = TIME_X_POS + pos_in_str[current_symbol] * font->width;

  hx8357_write_alignedX_string(x, TIME_Y_POS, str, font, COLOR_YELLOW, field_bg, ALIGN_LEFT);
}

void DateTime_Prepare (void)
{
  current_symbol = 0;
  HAL_RTC_GetTime(DATETIME_HANDLER, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(DATETIME_HANDLER, &sDate, RTC_FORMAT_BIN);
}

void DateTime_IncreaseIndex (void)
{
  DateTime_CharUpdate(COLOR_BLACK);

  current_symbol++;
  if (current_symbol >= CLOCK_MAX_SYMBOLS)
    current_symbol = 0;

  DateTime_CharUpdate(COLOR_BLUE);
  DateTime_change_status(MAX_UPDATE_STR_EMPTY);
}

void DateTime_DecreaseIndex (void)
{
  DateTime_CharUpdate(COLOR_BLACK);

  current_symbol--;
  if (current_symbol < 0)
    current_symbol = CLOCK_MAX_SYMBOLS - 1;

  DateTime_CharUpdate(COLOR_BLUE);
  DateTime_change_status(MAX_UPDATE_STR_EMPTY);
}

void DateTime_ChangeNumber (uint8_t n)
{
  datetimeBuffer[current_symbol] = n;
  bool overflow = DateTime_CheckSymbol();

  if (overflow) {
      Draw_MENU_TYPE_CLOCK();
  } else {
      DateTime_CharUpdate(COLOR_BLUE);
  }
  DateTime_change_status(MAX_UPDATE_STR_EMPTY);
}

void DateTime_Set (void)
{
  HAL_RTC_SetTime(DATETIME_HANDLER, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_SetDate(DATETIME_HANDLER, &sDate, RTC_FORMAT_BIN);

  DateTime_change_status(MAX_UPDATE_STR);
}

void DateTime_HeaderClock (void)
{
  RTC_TimeTypeDef sTime = { 0 };
  RTC_DateTypeDef sDate = { 0 };
  char clock_str[64];
  HAL_RTC_GetTime(DATETIME_HANDLER, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(DATETIME_HANDLER, &sDate, RTC_FORMAT_BIN);

  snprintf(clock_str, sizeof(clock_str), "%02d/%02d/%02d %02d:%02d:%02d", sDate.Date, sDate.Month, sDate.Year, sTime.Hours, sTime.Minutes, sTime.Seconds);

  hx8357_write_alignedX_string(0, TIME_HEADER_Y_POS, clock_str, &Font_11x18, COLOR_YELLOW, COLOR_BLACK, ALIGN_RIGHT);
}

void Draw_MENU_TYPE_CLOCK (void)
{
  FontDef *font = &Font_16x26;

  char buf[32];
  snprintf(buf, sizeof(buf), "%02d/%02d/%02d  %02d:%02d:%02d", sDate.Date, sDate.Month, sDate.Year, sTime.Hours, sTime.Minutes, sTime.Seconds);

  hx8357_write_alignedX_string(TIME_X_POS, TIME_Y_POS, buf, font, COLOR_YELLOW, COLOR_BLACK, ALIGN_LEFT);

  DateTime_CharUpdate(COLOR_BLUE);
}
