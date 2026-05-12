#include <keyboard.h>
#include "i2c.h"
#include "logger.h"

#define KEYBOARD_HANDLER &hi2c1

const char* ButtonToString (KeyCode_t code)
{
  switch (code)
  {
    case BTN_UP:
      return "BTN_UP";
    case BTN_DOWN:
      return "BTN_DOWN";
    case BTN_LEFT:
      return "BTN_LEFT";
    case BTN_RIGHT:
      return "BTN_RIGHT";
    case BTN_ENTER:
      return "BTN_ENTER";
    case BTN_ESC:
      return "BTN_ESC";
    case BTN_A:
      return "BTN_A";
    case BTN_B:
      return "BTN_B";
    case BTN_TEST:
      return "BTN_TEST";
    case BTN_ANNOUNCEMENT:
      return "BTN_ANNOUNCEMENT";
    case BTN_MESSAGE:
      return "BTN_MESSAGE";
    case BTN_ALARM:
      return "BTN_ALARM";
    case BTN_CXL:
      return "BTN_CXL";
    case BTN_ARM:
      return "BTN_ARM";
    case BTN_0:
      return "BTN_0";
    case BTN_1:
      return "BTN_1";
    case BTN_2:
      return "BTN_2";
    case BTN_3:
      return "BTN_3";
    case BTN_4:
      return "BTN_4";
    case BTN_5:
      return "BTN_5";
    case BTN_6:
      return "BTN_6";
    case BTN_7:
      return "BTN_7";
    case BTN_8:
      return "BTN_8";
    case BTN_9:
      return "BTN_9";
    case BTN_HASH:
      return "BTN_HASH";
    case BTN_ASTERISK:
      return "BTN_ASTERISK";
    case BTN_NONE:
      return "BTN_NONE";
    default:
      return "UNKNOWN_BUTTON";
  }
}

void keyboard_init (void)
{
  HAL_GPIO_WritePin(KEYPAD_RST_GPIO_Port, KEYPAD_RST_Pin, GPIO_PIN_SET);

  TCA8418_Init(KEYBOARD_HANDLER);

  for (int i = 0; i < KEYBOARD_ROW; i++)
  {
    TCA8418_SetKeypadPin(KEYBOARD_HANDLER, (TCA8418_Pin_t) i, true);
    TCA8418_SetEventFIFO(KEYBOARD_HANDLER, (TCA8418_Pin_t) i, true);
  }

  for (int i = 0; i < KEYBOARD_COL; i++)
  {
    TCA8418_SetKeypadPin(KEYBOARD_HANDLER, (TCA8418_Pin_t) (8 + i), true);
    TCA8418_SetEventFIFO(KEYBOARD_HANDLER, (TCA8418_Pin_t) (8 + i), true);
  }

  TCA8418_EnableKeyInterrupt(KEYBOARD_HANDLER, true);
}

void keyboard_process (KeyEvent_t *ev)
{
  uint8_t code;
  bool pressed;
  while (TCA8418_GetEventCount(KEYBOARD_HANDLER) > 0)
  {
    if (TCA8418_ReadKeyEvent(KEYBOARD_HANDLER, &code, &pressed) == HAL_OK)
    {
//      LOG_DEBUG("%s, code: %d, evt: %d", ButtonToString(code), code, pressed);
      ev->button = (KeyCode_t) code;
      ev->pressed = pressed;
    }
  }

  TCA8418_ClearInterrupt(KEYBOARD_HANDLER);
}
