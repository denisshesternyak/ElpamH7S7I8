#include "lcd_widget_amp_param.h"
#include <string.h>
#include "defines.h"
#include "hx8357d.h"
#include "fonts.h"
#include "analog.h"
#include <stdio.h>
#include "logger.h"
#include "metadata.h"

#define STATUS_BAR_LINE_Y_POS  		35

#define AMP_PARAM_TEXT_LINE1_Y 		190
#define AMP_PARAM_TEXT_LINE2_Y 		100
#define AMP_PARAM_VALUE_X		208
#define AMP_PARAM_VALUE_Y 		153

#define MAX_AMP_PARAM_SIZE 		4
#define MAX_UPDATE_STR			"UPDATED"
#define MAX_UPDATE_STR_EMPTY 		"       "

static char paramBuffer[MAX_AMP_PARAM_SIZE] = { 0 };
static int8_t param_index = 0;

static void param_change_status (const char *str)
{
  FontDef *font = &Font_11x18;
  hx8357_write_alignedX_string(0, AMP_PARAM_TEXT_LINE1_Y, str, font, COLOR_WHITE, COLOR_BLACK, ALIGN_CENTER);
}

void AmpParam_Init (void)
{
  uint16_t value = 0;
  if (!eeprom_read_object(EEPROM_SLOT_AMP_PARAM, &value, sizeof(value)))
  {
    LOG_ERROR("Failure to read AMP_PARAM from EEPROM");
    value = ADC_AMP_PARAM;
    eeprom_write_object(EEPROM_SLOT_AMP_PARAM, &value, sizeof(value));
  }
  else {
    LOG_INFO("Read AMP_PARAM from EEPROM: %d", value);
  }

//  value = ADC_AMP_PARAM;
//  eeprom_write_object(EEPROM_SLOT_AMP_PARAM, &value, sizeof(value));

  paramBuffer[0] = value / 1000;
  paramBuffer[1] = (value / 100) % 10;
  paramBuffer[2] = (value / 10) % 10;
  paramBuffer[3] = value % 10;

  param_index = 0;
  amp_param = value;
}

void AmpParam_IncreaseIndex (void)
{
  param_index++;
  if (param_index >= MAX_AMP_PARAM_SIZE)
    param_index = 0;

  AmpParam_UpdateValue();
  param_change_status(MAX_UPDATE_STR_EMPTY);
}

void AmpParam_DecreaseIndex (void)
{
  param_index--;
  if (param_index < 0)
    param_index = MAX_AMP_PARAM_SIZE - 1;

  AmpParam_UpdateValue();
  param_change_status(MAX_UPDATE_STR_EMPTY);
}

void AmpParam_ChangeNumber (uint8_t n)
{
  paramBuffer[param_index] = n;

  uint8_t num = (paramBuffer[0] * 1000) + (paramBuffer[1] * 100) + (paramBuffer[2] * 10) + paramBuffer[3];
  if (num > MAX_AMP_DRV_PARAMS)
  {
    uint16_t param = MAX_AMP_DRV_PARAMS;

    paramBuffer[0] = param / 1000;
    paramBuffer[1] = (param / 100) % 10;
    paramBuffer[2] = (param / 10) % 10;
    paramBuffer[3] = param % 10;
  }

  AmpParam_UpdateValue();
  param_change_status(MAX_UPDATE_STR_EMPTY);
}

static void param_write(uint16_t val)
{
  if (eeprom_write_object(EEPROM_SLOT_AMP_PARAM, &val, sizeof(val)))
  {
    amp_param = val;
    LOG_INFO("Save AMP_PARAM to EEPROM: %d", val);
  }
  else
    LOG_ERROR("Failure to save AMP_PARAM to EEPROM");
}

void AmpParam_SendAmpParam (void)
{
  // send to SPI
  uint16_t value = (paramBuffer[0] * 1000) + (paramBuffer[1] * 100) + (paramBuffer[2] * 10) + paramBuffer[3];
  param_change_status(MAX_UPDATE_STR);
  param_write(value);
}

void AmpParam_UpdateValue (void)
{
  FontDef *font = &Font_16x26;
  char buf[16];
  snprintf(buf, 16, "%1d%1d%1d%1d", paramBuffer[0], paramBuffer[1], paramBuffer[2], paramBuffer[3]);
  hx8357_write_alignedX_string(AMP_PARAM_VALUE_X, AMP_PARAM_VALUE_Y, buf, font, COLOR_YELLOW, COLOR_BLACK, ALIGN_LEFT);

  uint16_t field_color = COLOR_YELLOW;
  uint16_t field_bg = COLOR_BLUE;
  uint16_t field_offset = 0;

  const uint8_t pos_in_str[MAX_AMP_PARAM_SIZE] = { 0, 1, 2, 3 };
  field_offset = pos_in_str[param_index] * font->width;

  char field_str[4];
  snprintf(field_str, sizeof(field_str), "%1d", paramBuffer[param_index]);
  hx8357_write_alignedX_string(AMP_PARAM_VALUE_X + field_offset, AMP_PARAM_VALUE_Y, field_str, font, field_color, field_bg, ALIGN_LEFT);
}
