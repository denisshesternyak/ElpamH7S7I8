#include "command_dispatcher.h"
#include "system_status.h"
#include "defines.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "defines.h"  // For USE_DEBUG_COMMAND_DISPATCHER
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//#include "audio_types.h"
#include "lcd_menu.h"
#include "usart.h"
#include "stm32h7rsxx_hal.h"
#include "logger.h"
#include "app_freertos.h"
#include "audio.h"
#include "keyboard.h"

// Extern UART handler for debug output (optional)

static void send_btn_notify (KeyCode_t btn);
static void send_screen_notify (MenuType screen);

// Buffer for debug messages
#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    static char debug_msg[32];
    #include "../lcd/lcd_display.h"

   
#endif

// ————————————————————————————————————————
// Command handler functions
// Each function is called by rs232.c when a command is received
// ————————————————————————————————————————

/**
 * @brief Handle Arming Operation (*_ARM__)
 * Prepare the siren system for operation (e.g., self-test, power up)
 */
void handle_arm (void)
{
  system_status_set_mode(SYSTEM_MODE_ARMING);
  player.last_time_arming = 0;
  player.is_arming = true;
  LOG_DEBUG("ARM is started");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: ARM\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "B - ARM", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle All Clear 1 (*ACLR1_)
 * Deactivate alarm with clear signal type 1
 */
void handle_all_clear_1 (void)
{
  system_status_set_mode(SYSTEM_MODE_ALL_CLEAR_1);
  if (player.is_arming && !player.is_motorola)
  {
    audio_notify_high(AUDIO_STOP, player.type);
    audio_notify_high(AUDIO_START, AUDIO_MOTOROLA);
    send_screen_notify(MENU_TYPE_MOTOROLA);
  }

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: ALL CLEAR 1\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "F - ALL CLEAR 1", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle All Clear 2 (*ACLR2_)
 * Deactivate alarm with clear signal type 2
 */
void handle_all_clear_2 (void)
{
  system_status_set_mode(SYSTEM_MODE_ALL_CLEAR_2);
  if (player.is_arming && !player.is_motorola)
  {
    audio_notify_high(AUDIO_STOP, player.type);
    audio_notify_high(AUDIO_START, AUDIO_MOTOROLA);
    send_screen_notify(MENU_TYPE_MOTOROLA);
  }

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: ALL CLEAR 2\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "G - ALL CLEAR 2", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Alarm Operation (*IWAIL_)
 * Activate main alarm sound (wail)
 */
void handle_alarm (void)
{
  system_status_set_mode(SYSTEM_MODE_ALARM_WAIL);
  if (player.is_arming && !player.is_motorola)
  {
    audio_notify_high(AUDIO_STOP, player.type);
    audio_notify_high(AUDIO_START, AUDIO_MOTOROLA);
    send_screen_notify(MENU_TYPE_MOTOROLA);
  }

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: ALARM (WAIL)\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "H - ALARM", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Chemical Operation (*CHEM_A)
 * Activate alarm for chemical hazard
 */
void handle_chemical (void)
{
  system_status_set_mode(SYSTEM_MODE_CHEMICAL);
  if (player.is_arming && !player.is_motorola)
  {
    audio_notify_high(AUDIO_STOP, player.type);
    audio_notify_high(AUDIO_START, AUDIO_MOTOROLA);
    send_screen_notify(MENU_TYPE_MOTOROLA);
  }

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: CHEMICAL ALARM\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "L - CHEMICAL ALAR", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Disarm Command (*DISARM)
 * Immediately stop all active alarms and voice
 */
void handle_disarm (void)
{
  system_status_set_mode(SYSTEM_MODE_CANCEL_IMMEDIATE);
  player.is_arming = false;
  LOG_DEBUG("DISARM");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: DISARM\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "A - DISARM", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Cancel Command (*CANCEL)
 * Cancel current operation with delay
 */
void handle_cancel (void)
{
  if (player.is_motorola)
  {
    system_status_set_mode(SYSTEM_MODE_CANCEL_DELAYED);

    audio_notify_high(AUDIO_STOP, AUDIO_MOTOROLA);
    send_screen_notify(MENU_TYPE_PREVIOUS);
  }

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: CANCEL\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "C - CANCEL", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Quiet Test (*Q_TEST)
 * Perform silent test (e.g., check circuits without sound)
 */
void handle_quiet_test (void)
{
  system_status_set_mode(SYSTEM_MODE_QUIET_TEST);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: QUIET TEST\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "O - QUIET TEST", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Reserve 1 Command (*_WAIL_)
 * Reserved for future siren type
 */
void handle_reserve_1 (void)
{
  system_status_set_mode(SYSTEM_MODE_FUTURE_SIREN_1);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: RESERVE 1\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "I - RESERVE 1", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Reserve 2 Command (*PWAIL_)
 * Reserved for future siren type
 */
void handle_reserve_2 (void)
{
  system_status_set_mode(SYSTEM_MODE_FUTURE_SIREN_2);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: RESERVE 2\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "J - RESERVE 2", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Reserve 3 Command (*YELP__)
 * Reserved for future siren type
 */
void handle_reserve_3 (void)
{
  system_status_set_mode(SYSTEM_MODE_FUTURE_SIREN_3);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: RESERVE 3\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "K - RESERVE 3", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Remote P.A. Command (*_VOICE)
 * Activate voice public address mode
 */
void handle_remote_pa (void)
{
  system_status_set_mode(SYSTEM_MODE_VOICE);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: REMOTE PA\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "N - REMOTE PA", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Reset Command (*RESET_)
 * Reset system or protocol state
 */
void handle_reset (void)
{
  system_status_reset();

  //system_status_set_mode(SYSTEM_MODE_RESET);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: RESET\r\n");
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "E - RESET", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
  // Example: HAL_NVIC_SystemReset();
}

/**
 * @brief Handle Volume Up Command (*VOLnnn, nnn )
 * @param step - volume increase step
 */
void volume_up_handler (int value)
{
//  system_set_volume(value);
  player.volume = value;

  audio_notify_high(AUDIO_VOLUME, AUDIO_NONE);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "VOL UP: %d (now: %d)\r\n", value, system_get_volume());
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    sprintf(debug_msg, "VOL UP: %d (now: %d)", value, system_get_volume());
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, debug_msg, LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Volume Down Command (*VOLnnn, nnn >= 900)
 * @param step - volume decrease step (1..99 after conversion)
 */
void volume_down_handler (int value)
{
//  system_set_volume(value);
  player.volume = value;

  audio_notify_high(AUDIO_VOLUME, AUDIO_NONE);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "VOL DOWN: %d (now: %d)\r\n", value, system_get_volume());
    //HAL_UART_Transmit(&huart4, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
     sprintf(debug_msg, "VOL DOWN: %d (now: %d)", value, system_get_volume());
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, debug_msg, LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Fill 22-character status report for *REPORT command
 * @param response_22 - pointer to 22-byte buffer
 */
void system_fill_report ()
{
  char response_22[22] = { 0 };

  // Byte 1: Mode (one char)
  response_22[0] = system_status_get_mode_char();

  // Byte 2: Not used
  response_22[1] = '0';

  // Bytes 3–12: Driver 1–10
  for (int i = 0; i < 10; i++)
  {
    response_22[2 + i] = system_status.amplifier_driver[i] ? '1' : '0';
  }

  // Byte 13: Not used
  response_22[12] = '0';

  // Byte 14: Max Volume
  response_22[13] = system_status.max_volume ? '1' : '0';

  // Byte 15: Main Tone
  response_22[14] = system_status.main_tone ? '1' : '0';

  // Byte 16: Secondary Tone
  response_22[15] = system_status.secondary_tone ? '1' : '0';

  // Byte 17: Operating Current
  response_22[16] = system_status.operating_current ? '1' : '0';

  // Byte 18: Battery Voltage
  response_22[17] = system_status.battery_voltage ? '1' : '0';

  // Byte 19: Charger Unit
  response_22[18] = system_status.charger_unit ? '1' : '0';

  // Byte 20: AC Voltage
  response_22[19] = system_status.ac_voltage ? '1' : '0';

  // Byte 21: Flood Sensor
  response_22[20] = system_status.flood_sensor ? '1' : '0';

  // Byte 22: Door Sensor
  response_22[21] = system_status.door_sensor ? '1' : '0';

  HAL_UART_Transmit(&huart4, (uint8_t*) response_22, 22, HAL_MAX_DELAY);
}

/**
 * @brief Handle unknown or malformed command
 */
void handle_unknown_command (void)
{
  LOG_DEBUG("Unknown command");
#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    ///HAL_UART_Transmit(&huart4, (uint8_t*)"ERR:UNKNOWN\r\n", 13, HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "ERR:UNKNOWN", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

void handle_enter_command (void)
{
  send_btn_notify(BTN_ENTER);
}
void handle_up_command (void)
{
  send_btn_notify(BTN_UP);
}
void handle_down_command (void)
{
  send_btn_notify(BTN_DOWN);
}
void handle_esc_command (void)
{
  send_btn_notify(BTN_ESC);
}
void handle_cancel_command (void)
{
  send_btn_notify(BTN_CXL);
}
void handle_test_command (void)
{
  send_btn_notify(BTN_TEST);
}
void handle_announc_command (void)
{
  send_btn_notify(BTN_ANNOUNCEMENT);
}
void handle_message_command (void)
{
  send_btn_notify(BTN_MESSAGE);
}
void handle_alarm_command (void)
{
  send_btn_notify(BTN_ALARM);
}
void handle_arm_command (void)
{
  send_btn_notify(BTN_ARM);
}

static void send_btn_notify (KeyCode_t btn)
{
  player.priority = AUDIO_PRIORITY_LOW;

  LCDTaskEvent_t lcd_event = { .event = LCD_EVENT_BTN, .btn = {
      .button = btn,
      .pressed = true } };

  if (osMessageQueuePut(xLCDQueueHandle, &lcd_event, 0, 0) != osOK)
  {
    LOG_WARN("The lcd queue is full");
  }
}

static void send_screen_notify (MenuType screen)
{
  LCDTaskEvent_t lcd_event = { .event = LCD_EVENT_SCREEN };
  lcd_event.screen = screen;

  if (osMessageQueuePut(xLCDQueueHandle, &lcd_event, 0, 0) != osOK)
  {
    LOG_WARN("The lcd queue is full");
  }
}
