#ifndef SRC_LCD_LCD_WIDGET_VOLUME_INDICATOR_H_
#define SRC_LCD_LCD_WIDGET_VOLUME_INDICATOR_H_

#include <stdint.h>
//#include "defines.h"

#define  VOLUME_BARS_W			396
#define  VOLUME_BAR_SPACING_X 		3
#define  VOLUME_BAR_SPACING_Y 		8
#define  VOLUME_OUTLINE_SIZE 		1
#define  VOLUME_BAR_W			26 //((VOLUME_BARS_W - (VOLUME_BAR_SPACING_X*2)) / NUM_VOLUME_BARS)
#define  VOLUME_BAR_H			30
#define  VOLUME_BAR_X			42 //(((480 - VOLUME_BARS_W) / 2) - VOLUME_BAR_W)
#define  VOLUME_BAR_Y			250

#define NUM_VALID_LEVELS    15

#define NUM_VOLUME_BARS     NUM_VALID_LEVELS   // Number of bars on the indicator

#define MAX_MENU_ITEMS 	25

typedef struct
{
  uint16_t x;                 ///< X coordinate of the top-left corner on screen
  uint16_t y;                 ///< Y coordinate of the top-left corner on screen
  uint16_t width;               ///< Total width of the indicator in pixels
  uint16_t height;              ///< Total height of the indicator in pixels
  uint8_t numBars; ///< Number of individual bars in the indicator (e.g., 6, 10)
  uint16_t barColor;            ///< Color of active (filled) bars
  uint16_t bgColor; ///< Background color outside the inner area (rarely used directly)
  uint16_t inactiveBgColor;     ///< Color of inactive (empty) bar positions
  uint16_t frameColor;       ///< Color of the border frame around the indicator
  uint8_t level;        ///< Current level (number of active bars, 0 to numBars)
  uint8_t old_level;	///< Old level (number of active bars, 0 to numBars)
  uint8_t volume_value;		  ///< Current value (value of volume)
  uint8_t frameWidth;           ///< Width of the border frame in pixels
  uint8_t barSpacing;           ///< Spacing in pixels between individual bars
} VolumeIndicator_t;

extern VolumeIndicator_t volumeIndicator;

void volume_indicators_draw ();
void volume_indicator_set_level (uint8_t level);
void volume_indicator_set_level_silent (uint8_t level, uint8_t val);
void volume_indicator_draw_bar (uint8_t level, uint8_t val);
void volume_indicator_blink_bar ();

//void volume_indicator_step_up(void);
//void volume_indicator_step_down(void);

#endif /* SRC_LCD_LCD_WIDGET_VOLUME_INDICATOR_H_ */
