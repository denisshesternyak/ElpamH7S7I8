#include <keyboard.h>
#include "i2c.h"
//#include "logger.h"

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
    case BTN_NONE:
      return "BTN_NONE";
    default:
      return "UNKNOWN_BUTTON";
  }
}

void keyboard_init (void)
{

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
  while (TCA8418_GetEventCount(KEYBOARD_HANDLER) > 0)
  {
    if (TCA8418_ReadKeyEvent(KEYBOARD_HANDLER, ev) == HAL_OK)
    {
      // Event press button
    }
  }

  TCA8418_ClearInterrupt(KEYBOARD_HANDLER);
}
