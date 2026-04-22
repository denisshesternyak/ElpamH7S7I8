/*
 * lcd_color_rgb565.h
 *
 *  Created on: Jun 20, 2025
 *      Author: roman
 */

#ifndef INC_LCD_LCD_COLOR_RGB565_H_
#define INC_LCD_LCD_COLOR_RGB565_H_


// =================================================
//           RGB565 Color Definitions (16-bit)
// =================================================
// Format: 0bRRRRRGGGGGGBBBBB (5-6-5 bits)
// Usage: uint16_t color = COLOR_RED;
#define RGB888_TO_RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

// Basic Colors
#define COLOR_RED      0xF800  // 0b1111100000000000 (R=31, G=0,  B=0)
#define COLOR_GREEN    0x07E0  // 0b0000011111100000 (R=0,  G=63, B=0)
#define COLOR_BLUE     0x001F  // 0b0000000000011111 (R=0,  G=0,  B=31)
#define COLOR_WHITE    0xFFFF  // 0b1111111111111111 (R=31, G=63, B=31)
#define COLOR_BLACK    0x0000  // 0b0000000000000000 (R=0,  G=0,  B=0)
#define COLOR_YELLOW   0xFFE0  // 0b1111111111100000 (R=31, G=63, B=0)
#define COLOR_CYAN     0x07FF  // 0b0000011111111111 (R=0,  G=63, B=31)
#define COLOR_MAGENTA  0xF81F  // 0b1111100000011111 (R=31, G=0,  B=31)

// Grayscale (approximate)
#define COLOR_GRAY     0x8410  // 0b1000010000010000 (R=16, G=32, B=16)
#define COLOR_LIGHTGRAY 0xC618 // 0b1100011000011000 (R=24, G=48, B=24)
#define COLOR_DARKGRAY 0x4208  // 0b0100001000001000 (R=8,  G=16, B=8)

// Extended Colors
#define COLOR_ORANGE   0xFD20  // 0b1111110100100000 (R=31, G=41, B=0)
#define COLOR_PURPLE   0x8010  // 0b1000000000010000 (R=16, G=0,  B=16)
#define COLOR_PINK     0xFC1F  // 0b1111110000011111 (R=31, G=0,  B=31, brighter)
#define COLOR_LIME     0x87E0  // 0b1000011111100000 (R=16, G=63, B=0)
#define COLOR_TEAL     0x0410  // 0b0000010000010000 (R=0,  G=32, B=16)
#define COLOR_NAVY     0x0010  // 0b0000000000010000 (R=0,  G=0,  B=16)

// Special Colors (OLED-friendly)
#define COLOR_OLIVE    0x8400  // 0b1000010000000000 (R=16, G=32, B=0)
#define COLOR_MAROON   0x8000  // 0b1000000000000000 (R=16, G=0,  B=0)

#endif /* INC_LCD_LCD_COLOR_RGB565_H_ */
