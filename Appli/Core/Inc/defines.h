

#ifndef INC_DEFINES_H_
#define INC_DEFINES_H_

#define USE_LCD_ILI_9488   // 480x320

#define VER_MAJOR  		0
#define VER_MINOR  		14

#define LCD_USART_TEXT_Y        300
#define LCD_USART_TEXT_X        5
#define LCD_USART_TEXT_FONT    	&Font_11x18


//#define MAX_VOLUME_CODEC    0
//#define MIN_VOLUME_CODEC    200
#define MAX_VOLUME_CODEC    	0
#define MIN_VOLUME_CODEC    	40
#define MAX_VOLUME          	122
#define MIN_VOLUME          	80
#define VOLUME_STEP         	3
#define DEF_VALUE_VOLUME    	80

#define NUM_VALID_LEVELS    	15

#define NUM_VOLUME_BARS     	NUM_VALID_LEVELS

#define MAX_MENU_ITEMS 		25

#endif /* INC_DEFINES_H_ */
