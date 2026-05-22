#include "lcd_widget_volume.h"
#include <string.h>
#include "defines.h"
#include "hx8357d.h"
#include "fonts.h"
#include <stdio.h>

#define STATUS_BAR_LINE_Y_POS  		35

#define VOLUME_TEXT_LINE1_Y 		190
#define VOLUME_TEXT_LINE2_Y 		100
#define VOLUME_VALUE_X			208
#define VOLUME_VALUE_Y 			153

#define MAX_VOLUME_SIZE 		3
#define MAX_UPDATE_STR			"UPDATED"
#define MAX_UPDATE_STR_EMPTY 		"       "

static char volumeBuffer[MAX_VOLUME_SIZE] = { 0 };
static int8_t volumeIndex = 0;

static void volume_change_status (const char *str)
{
  FontDef *font = &Font_11x18;
  hx8357_write_alignedX_string(0, VOLUME_TEXT_LINE1_Y, str, font, COLOR_WHITE, COLOR_BLACK, ALIGN_CENTER);
}

void Volume_IncreaseIndex (void)
{
  volumeIndex++;
  if(volumeIndex >= MAX_VOLUME_SIZE)
    volumeIndex = 0;

  Volume_UpdateValue();
  volume_change_status(MAX_UPDATE_STR_EMPTY);
}

void Volume_DecreaseIndex (void)
{
  volumeIndex--;
  if(volumeIndex < 0)
    volumeIndex = MAX_VOLUME_SIZE-1;

  Volume_UpdateValue();
  volume_change_status(MAX_UPDATE_STR_EMPTY);
}

void Volume_ChangeNumber (uint8_t n)
{
  volumeBuffer[volumeIndex] = n;

  Volume_UpdateValue();
  volume_change_status(MAX_UPDATE_STR_EMPTY);
}

void Volume_SendVolume (void)
{
  // send to SPI
  volume_change_status(MAX_UPDATE_STR);
}

void Volume_UpdateValue (void)
{
  FontDef *font = &Font_16x26;
  char buf[16];
  snprintf(buf, 16, "%1d.%1d%1d", volumeBuffer[0], volumeBuffer[1], volumeBuffer[2]);
  hx8357_write_alignedX_string(VOLUME_VALUE_X, VOLUME_VALUE_Y, buf, font, COLOR_YELLOW, COLOR_BLACK, ALIGN_LEFT);

  uint16_t field_color = COLOR_YELLOW;
  uint16_t field_bg = COLOR_BLUE;
  uint16_t field_offset = 0;

  const uint8_t pos_in_str[MAX_VOLUME_SIZE] = { 0, 2, 3 };
  field_offset = pos_in_str[volumeIndex] * font->width;

  char field_str[4];
  snprintf(field_str, sizeof(field_str), "%1d", volumeBuffer[volumeIndex]);
  hx8357_write_alignedX_string(VOLUME_VALUE_X + field_offset, VOLUME_VALUE_Y, field_str, font, field_color, field_bg, ALIGN_LEFT);
}
