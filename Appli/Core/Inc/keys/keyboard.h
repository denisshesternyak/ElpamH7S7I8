#ifndef INC_KEYS_KEYBOARD_H_
#define INC_KEYS_KEYBOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include "tca8418.h"

#define KEYBOARD_ROW 	5
#define KEYBOARD_COL	6

//      Col0  Col1  Col2  Col3  Col4  Col5
// Row0:  1     2     3      4     5     6
// Row1:  7     8     9     10    11    12
// Row2: 13    14    15     16    17    18
// Row3: 19    20    21     22    23    19
// Row4: 25    26    27     28    29    30

typedef enum
{
  BTN_RIGHT = 3,
  BTN_LEFT = 4,
  BTN_ARM = 5,
  BTN_CXL = 6,
  BTN_ESC = 13,
  BTN_UP = 14,
  BTN_DOWN = 15,
  BTN_ENTER = 16,

  //------------------------
  BTN_TEST = 33,
  BTN_ANNOUNCEMENT = 34,
  BTN_MESSAGE = 35,
  BTN_ALARM = 26,

  //------------------------
  BTN_0 = 31,
  BTN_2 = 1,
  BTN_3 = 2,
  BTN_5 = 11,
  BTN_6 = 12,
  BTN_8 = 21,
  BTN_9 = 22,
  BTN_HASH = 32, // #

  BTN_1 = 99,
  BTN_4,
  BTN_7,
  BTN_ASTERISK, // *

  BTN_A,
  BTN_B,
  BTN_NONE
} KeyCode_t;

typedef struct
{
  bool pressed;
  KeyCode_t button;
} KeyEvent_t;

void keyboard_init (void);
void keyboard_process (KeyEvent_t *ev);
const char* ButtonToString (KeyCode_t btn);

#endif /* INC_KEYS_KEYBOARD_H_ */
