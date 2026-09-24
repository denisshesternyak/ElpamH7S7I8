#ifndef INC_KEYS_KEYBOARD_H_
#define INC_KEYS_KEYBOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include "tca8418.h"

#define KEYBOARD_ROW 	4
#define KEYBOARD_COL	7

//      Col0  Col1  Col2  Col3  Col4  Col5
// Row0:  1     2     3      4     5     6
// Row1:  7     8     9     10    11    12
// Row2: 13    14    15     16    17    18
// Row3: 19    20    21     22    23    19
// Row4: 25    26    27     28    29    30

typedef enum
{
  BTN_RIGHT = 4,
  BTN_LEFT = 5,
  BTN_ARM = 6,
  BTN_CXL = 7,
  BTN_ESC = 14,
  BTN_UP = 15,
  BTN_DOWN = 16,
  BTN_ENTER = 17,

  //------------------------
  BTN_TEST = 34,
  BTN_ANNOUNCEMENT = 35,
  BTN_MESSAGE = 36,

  //------------------------

  BTN_0 = 32,
  BTN_1 = 1,
  BTN_2 = 2,
  BTN_3 = 3,
  BTN_4 = 11,
  BTN_5 = 12,
  BTN_6 = 13,
  BTN_7 = 21,
  BTN_8 = 22,
  BTN_9 = 23,
  BTN_ASTERISK = 31, // *
  BTN_HASH = 33, // #

  BTN_ALARM = 70,
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
