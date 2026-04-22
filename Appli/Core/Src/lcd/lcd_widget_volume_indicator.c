#include "lcd_widget_volume_indicator.h"
#include "hx8357d.h"
#include <stdio.h>
#include <stdbool.h>

VolumeIndicator_t volumeIndicator;
static char debug_msg[32];

static void volume_indicator_draw_string(void)
{
	sprintf(debug_msg, "Vol: %-3d dB, Level: %-3d", volumeIndicator.volume_value, volumeIndicator.level);
	hx8357_write_alignedX_string(0, 220, debug_msg, &Font_11x18, COLOR_YELLOW, COLOR_BLACK, ALIGN_CENTER);
}

static void draw_bar(uint8_t index, uint16_t color)
{
    const uint16_t barH = VOLUME_BAR_H - (VOLUME_BAR_SPACING_Y * 2);
    const uint16_t barX = volumeIndicator.x + (index * VOLUME_BAR_W) + VOLUME_BAR_SPACING_X;
    const uint16_t barY = VOLUME_BAR_Y + VOLUME_BAR_SPACING_Y;
    hx8357_fill_rect(barX, barY, VOLUME_BAR_W - VOLUME_BAR_SPACING_X, barH, color);
}

static void fill_single_bar(uint8_t index, bool isVisible)
{
	uint16_t color = isVisible ? COLOR_BLUE : COLOR_BLACK;

	if(volumeIndicator.old_level != volumeIndicator.level)
	{
		index = volumeIndicator.old_level-1;
		if(volumeIndicator.old_level > volumeIndicator.level)
		{
			color = COLOR_BLACK;
		}
		else if(volumeIndicator.old_level < volumeIndicator.level)
		{
			draw_bar(index, COLOR_BLUE);
		}
		volumeIndicator.old_level = volumeIndicator.level;
	}

	draw_bar(index, color);
}

void volume_indicators_draw(void)
{
	hx8357_draw_rect(VOLUME_BAR_X, VOLUME_BAR_Y, VOLUME_BARS_W, VOLUME_BAR_H, VOLUME_OUTLINE_SIZE, COLOR_WHITE, COLOR_BLACK);

	volumeIndicator.x = VOLUME_BAR_X;
	volumeIndicator.y = VOLUME_BAR_Y;
	volumeIndicator.old_level = volumeIndicator.level;

	for (uint8_t i = 0; i < volumeIndicator.level; i++) {
		fill_single_bar(i, true);
	}
	volume_indicator_draw_string();
}

void volume_indicator_set_level_silent(uint8_t level, uint8_t val)
{
    if (level > NUM_VOLUME_BARS) level = NUM_VOLUME_BARS;
    volumeIndicator.old_level = volumeIndicator.level;
    volumeIndicator.level = level;
    volumeIndicator.volume_value = val;
}

void volume_indicator_draw_bar(uint8_t level, uint8_t val)
{
	volume_indicator_set_level_silent(level, val);
    fill_single_bar(level-1, true);
    volume_indicator_draw_string();
}

void volume_indicator_blink_bar()
{
	static bool blink = true;
    fill_single_bar(volumeIndicator.level-1, blink);
    blink = !blink;
}

//void volume_indicator_step_up()
//{
//    if (volumeIndicator.level >= NUM_VOLUME_BARS) return;
//    fill_single_bar(volumeIndicator.level);
//}
//
//void volume_indicator_step_down()
//{
//    if (level == 0) return;
//    volumeIndicator.level = level;
//    fill_single_bar(level);
//}


