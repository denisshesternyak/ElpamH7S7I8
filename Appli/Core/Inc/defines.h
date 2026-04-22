

#ifndef INC_DEFINES_H_
#define INC_DEFINES_H_

// choose only one define !!!!
// Select LCD type screen

//#define USE_LCD_ILI9341  // 320x240
#define USE_LCD_ILI_9488   // 480x320
//#define USE_LCD_ST7796   // 480x320


// Uncomment to enable debug output in command_dispatcher
//#define USE_DEBUG_COMMAND_DISPATCHER
#define LCD_USART_TEXT_Y        300
#define LCD_USART_TEXT_X        5
#define LCD_USART_TEXT_FONT    &Font_11x18
//#define LCD_USART_TEXT_FONT    &Font_7x10


//#define MAX_VOLUME_CODEC    0
//#define MIN_VOLUME_CODEC    200
#define MAX_VOLUME_CODEC    0
#define MIN_VOLUME_CODEC    40
#define MAX_VOLUME          122     // dB
#define MIN_VOLUME          80      // dB
#define VOLUME_STEP         3      // dB — increment/decrement step
#define DEF_VALUE_VOLUME    80     // default value volume

#define NUM_VALID_LEVELS    15

#define NUM_VOLUME_BARS     NUM_VALID_LEVELS   // Number of bars on the indicator

#define MAX_MENU_ITEMS 			25

#endif /* INC_DEFINES_H_ */
