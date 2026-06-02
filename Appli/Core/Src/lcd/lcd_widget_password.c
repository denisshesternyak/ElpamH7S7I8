#include "lcd_widget_password.h"
#include <string.h>
#include <stdint.h>
#include "defines.h"
#include "hx8357d.h"
#include "fonts.h"
#include <stdio.h>

static bool passwordIsCorrect = false;

#define STATUS_BAR_LINE_Y_POS  35

#define PASSWORD_TEXT_LINE1_Y 80
#define PASSWORD_TEXT_LINE2_Y 100
#define PASSWORD_BOX_Y 150
#define PASSWORD_BOX_SIZE 40
#define PASSWORD_BOX_SPACING 20

#define MAX_PASSWORD_LENGTH 6

static uint8_t passwordLength = 4;
static char passwordBuffer[MAX_PASSWORD_LENGTH] = { 0 };
static uint8_t passwordIndex = 0;
static const char correctPassword[] = MAINTENANCE_PASSWORD;

bool Password_IsCorrect (void)
{
  return passwordIsCorrect;
}

void OnPasswordComplete (bool success);
void DrawPasswordBox (uint8_t index, bool filled);

void Password_AddChar (char c)
{
  if (passwordIndex >= passwordLength)
    return;

  passwordBuffer[passwordIndex++] = c;

  DrawPasswordBox(passwordIndex - 1, true);

  if (passwordIndex < passwordLength)
    DrawPasswordBox(passwordIndex, false);

//    if (passwordIndex == passwordLength) {
//        bool match = (strncmp(passwordBuffer, correctPassword, passwordLength) == 0);
//        OnPasswordComplete(match);
//    }
}

void Password_Backspace (void)
{
  if (passwordIndex == 0)
    return;

  DrawPasswordBox(passwordIndex, false);

  passwordIndex--;
  passwordBuffer[passwordIndex] = 0;

  DrawPasswordBox(passwordIndex, false);
}

void Password_Reset (bool redraw)
{
  memset(passwordBuffer, 0, sizeof(passwordBuffer));
  passwordIndex = 0;
  passwordIsCorrect = false;
  if (redraw)
  {
    Draw_MENU_TYPE_PASSWORD();
  }

}

void Password_Enter (void)
{
  if (passwordIndex < passwordLength)
  {
    FontDef *font = &Font_11x18;
    hx8357_write_alignedX_string(0, PASSWORD_BOX_Y + PASSWORD_BOX_SIZE + 20, "Incomplete password", font, COLOR_BLACK, COLOR_WHITE, ALIGN_CENTER);
    return;
  }

  bool match = (strncmp(passwordBuffer, correctPassword, passwordLength) == 0);
  OnPasswordComplete(match);
}

void DrawPasswordBox (uint8_t index, bool filled)
{
  if (index >= passwordLength)
    return;

  FontDef *font = &Font_11x18;

  uint16_t totalWidth = PASSWORD_BOX_SIZE * passwordLength + PASSWORD_BOX_SPACING * (passwordLength - 1);
  uint16_t startX = (hx8357_get_width() - totalWidth) / 2;
  uint16_t x = startX + index * (PASSWORD_BOX_SIZE + PASSWORD_BOX_SPACING);

  uint16_t fillColor =
      (index == passwordIndex && passwordIndex < passwordLength) ? COLOR_HIGHLIGHT : COLOR_LIGHTGRAY;

  hx8357_fill_rect(x, PASSWORD_BOX_Y, PASSWORD_BOX_SIZE, PASSWORD_BOX_SIZE, fillColor);

  if (filled)
  {
    hx8357_write_char(x + (PASSWORD_BOX_SIZE / 2) - 5, PASSWORD_BOX_Y + 15, '*', font, COLOR_BLACK, fillColor);
  }
}

void Draw_MENU_TYPE_PASSWORD (void)
{
  //LCD_FillScreen(COLOR_WHITE);
//  LCD_FillRectangle(0, STATUS_BAR_LINE_Y_POS + 2, LCD_GetWidth(), LCD_GetHeight(), COLOR_WHITE);

  FontDef *font = &Font_11x18;

  hx8357_write_alignedX_string(0, PASSWORD_TEXT_LINE1_Y, "Enter password to", font, COLOR_WHITE, COLOR_BLACK, ALIGN_CENTER);
  hx8357_write_alignedX_string(0, PASSWORD_TEXT_LINE2_Y, "continue", font, COLOR_WHITE, COLOR_BLACK, ALIGN_CENTER);

  char buf_ver[32];
  snprintf(buf_ver, sizeof(buf_ver), "v%d.%d", VER_MAJOR, VER_MINOR);
  hx8357_write_alignedX_string(0, 284, buf_ver, &Font_16x26, COLOR_YELLOW, COLOR_BLACK, ALIGN_CENTER);

  for (int i = 0; i < passwordLength; i++)
  {
    bool filled = i < passwordIndex;
    DrawPasswordBox(i, filled);
  }
}

void OnPasswordComplete (bool success)
{
  FontDef *font = &Font_11x18;
  passwordIsCorrect = success;

  if (success)
  {
    hx8357_write_alignedX_string(0, PASSWORD_BOX_Y + PASSWORD_BOX_SIZE + 20, "Access Granted", font, COLOR_BLACK, COLOR_WHITE, ALIGN_CENTER);
  }
  else
  {
    hx8357_write_alignedX_string(0, PASSWORD_BOX_Y + PASSWORD_BOX_SIZE + 20, "Access Denied", font, COLOR_BLACK, COLOR_WHITE, ALIGN_CENTER);
//    osDelay(1000);
    Password_Reset(false);
  }
}
