#include "lcd_widget_driver.h"
#include <string.h>
#include "defines.h"
#include "audio.h"
#include <stdio.h>
#include "logger.h"
#include "metadata.h"

static uint8_t current_driver = 0;

static const uint8_t kingstar_drv[3] = {25, 55, 83};
static const uint8_t atlas_drv[3] = {35, 70, 113};
static const uint8_t jingsu_drv[3] = {5, 35, 43};
static const uint8_t *drivers[3] = {kingstar_drv, atlas_drv, jingsu_drv};

void Read_Current_Driver (void)
{
  uint8_t value;
  if (!eeprom_read_object(EEPROM_SLOT_DRIVER, &value, sizeof(value)))
  {
    LOG_ERROR("Failure to read driver from EEPROM");
    value = 0;
  }
  else {
    LOG_INFO("Read driver from EEPROM: %d", value);
  }

  current_driver = value;
}

uint8_t Get_Current_Driver (void)
{
  return current_driver;
}

void Set_Current_Driver (uint8_t value)
{
  if (eeprom_write_object(EEPROM_SLOT_DRIVER, &value, sizeof(value)))
  {
    LOG_INFO("Save driver to EEPROM: %d", value);
    current_driver = value;
  }
  else
    LOG_ERROR("Failure to save driver to EEPROM");
}

uint8_t get_min_ohm (void)
{
  return drivers[current_driver][0];
}

uint8_t get_mid_ohm (void)
{
  return drivers[current_driver][1];
}

uint8_t get_max_ohm (void)
{
  return drivers[current_driver][2];
}

