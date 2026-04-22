#include "lcd_widget_motorola.h"
#include "hx8357d.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void motorola_draw()
{
	char name[32] = "Choma & Migdal";
	hx8357_write_alignedX_string(0, 150, name, &Font_16x26, COLOR_MAGENTA, COLOR_BLACK, ALIGN_CENTER);
}

void motorola_update()
{
	char play_str[32] = "Playing...";
	size_t len = strlen(play_str);

	static bool blink = true;
	FontDef* font = &Font_16x26;
	uint16_t Y = 200;

	if (blink)
	{
		hx8357_write_alignedX_string(0, Y, play_str, font, COLOR_YELLOW, COLOR_BLACK, ALIGN_CENTER);
	}
	else
	{
		hx8357_fill_rect(160, Y, font->width*len, font->height, COLOR_BLACK);
	}

	blink = !blink;
}
