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
#include "lcd_types.h"
#include "usart.h"
#include "stm32h7rsxx_hal.h"
#include "logger.h"
#include "app_freertos.h"
#include "keyboard.h"

// Extern UART handler for debug output (optional)

static void send_btn_notify (KeyCode_t btn);
static void notify_arming (bool val);
static void notify_audio (AudioEvent_t event, AudioType_t type);

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
void handle_arm (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_ARMING);
  notify_audio(AUDIO_STOP, AUDIO_CURRENT_TYPE);
  notify_arming(true);
  LOG_INFO("'*_ARM__' - got the command");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: ARM\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "B - ARM", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle All Clear 1 (*ACLR1_)
 * Deactivate alarm with clear signal type 1
 */
void handle_all_clear_1 (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_ALL_CLEAR_1);
  LOG_INFO("'*A_CLR1' - got the command");
  notify_audio(AUDIO_START, AUDIO_MOTOROLA);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: ALL CLEAR 1\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "F - ALL CLEAR 1", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle All Clear 2 (*ACLR2_)
 * Deactivate alarm with clear signal type 2
 */
void handle_all_clear_2 (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_ALL_CLEAR_2);
  LOG_INFO("'*A_CLR2' - got the command");
  notify_audio(AUDIO_START, AUDIO_MOTOROLA);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: ALL CLEAR 2\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "G - ALL CLEAR 2", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Alarm Operation (*IWAIL_)
 * Activate main alarm sound (wail)
 */
void handle_alarm (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_ALARM_WAIL);

    LOG_INFO("'*I_WAIL' - got the command");
    notify_audio(AUDIO_START, AUDIO_MOTOROLA);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: ALARM (WAIL)\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "H - ALARM", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Chemical Operation (*CHEM_A)
 * Activate alarm for chemical hazard
 */
void handle_chemical (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_CHEMICAL);
  LOG_INFO("'*CHEM_A' - got the command");
  notify_audio(AUDIO_START, AUDIO_MOTOROLA);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: CHEMICAL ALARM\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "L - CHEMICAL ALAR", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Disarm Command (*DISARM)
 * Immediately stop all active alarms and voice
 */
void handle_disarm (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_CANCEL_IMMEDIATE);
  notify_audio(AUDIO_STOP, AUDIO_MOTOROLA);
  notify_arming(false);
  LOG_INFO("'*DISARM' - got the command");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: DISARM\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "A - DISARM", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Cancel Command (*CANCEL)
 * Cancel current operation with delay
 */
void handle_cancel (UART_HandleTypeDef *huart)
{
    LOG_INFO("'*CANCEL' - got the command");
    system_status_set_mode(SYSTEM_MODE_CANCEL_DELAYED);
    notify_audio(AUDIO_STOP, AUDIO_MOTOROLA);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: CANCEL\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "C - CANCEL", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Quiet Test (*Q_TEST)
 * Perform silent test (e.g., check circuits without sound)
 */
void handle_quiet_test (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_QUIET_TEST);
  LOG_INFO("'*Q_TEST' - got the command");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: QUIET TEST\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "O - QUIET TEST", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Reserve 1 Command (*_WAIL_)
 * Reserved for future siren type
 */
void handle_reserve_1 (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_FUTURE_SIREN_1);
  LOG_INFO("'*_WAIL_' - got the command");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: RESERVE 1\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "I - RESERVE 1", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Reserve 2 Command (*PWAIL_)
 * Reserved for future siren type
 */
void handle_reserve_2 (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_FUTURE_SIREN_2);
  LOG_INFO("'*PWAIL_' - got the command");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: RESERVE 2\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "J - RESERVE 2", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Reserve 3 Command (*YELP__)
 * Reserved for future siren type
 */
void handle_reserve_3 (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_FUTURE_SIREN_3);
  LOG_INFO("'*YELP__' - got the command");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: RESERVE 3\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "K - RESERVE 3", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Remote P.A. Command (*_VOICE)
 * Activate voice public address mode
 */
void handle_remote_pa (UART_HandleTypeDef *huart)
{
  system_status_set_mode(SYSTEM_MODE_VOICE);
  LOG_INFO("'*_VOICE' - got the command");

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: REMOTE PA\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "N - REMOTE PA", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Handle Reset Command (*RESET_)
 * Reset system or protocol state
 */
void handle_reset (UART_HandleTypeDef *huart)
{
  system_status_reset();
  LOG_INFO("'*_VOICE' - got the command");

  //system_status_set_mode(SYSTEM_MODE_RESET);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "CMD: RESET\r\n");
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
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
//  player.volume_value = value;

  notify_audio(AUDIO_VOLUME, AUDIO_NONE);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "VOL UP: %d (now: %d)\r\n", value, system_get_volume());
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
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
//  player.volume_value = value;

  notify_audio(AUDIO_VOLUME, AUDIO_NONE);

#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    //sprintf(debug_msg, "VOL DOWN: %d (now: %d)\r\n", value, system_get_volume());
    //HAL_UART_Transmit(huart, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
     sprintf(debug_msg, "VOL DOWN: %d (now: %d)", value, system_get_volume());
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, debug_msg, LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

/**
 * @brief Fill 22-character status report for *REPORT command
 * @param response_22 - pointer to 22-byte buffer
 */
void system_fill_report (UART_HandleTypeDef *huart)
{
  LOG_INFO("'*REPORT' - got the command");

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

  HAL_UART_Transmit(huart, (uint8_t*) response_22, 22, HAL_MAX_DELAY);
}

/**
 * @brief Handle unknown or malformed command
 */
void handle_unknown_command (UART_HandleTypeDef *huart)
{
  LOG_DEBUG("Unknown command");
#if defined(USE_DEBUG_COMMAND_DISPATCHER)
    ///HAL_UART_Transmit(huart, (uint8_t*)"ERR:UNKNOWN\r\n", 13, HAL_MAX_DELAY);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "                             ", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
    LCD_WriteString(LCD_USART_TEXT_X, LCD_USART_TEXT_Y, "ERR:UNKNOWN", LCD_USART_TEXT_FONT, COLOR_YELLOW, COLOR_BLACK);
#endif
}

void handle_enter_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_ENTER);
}
void handle_up_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_UP);
}
void handle_down_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_DOWN);
}
void handle_esc_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_ESC);
}
void handle_cancel_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_CXL);
}
void handle_test_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_TEST);
}
void handle_announc_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_ANNOUNCEMENT);
}
void handle_message_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_MESSAGE);
}
void handle_alarm_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_ALARM);
}
void handle_arm_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_ARM);
}

void handle_amp_t1_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T1_ACT_GPIO_Port, AMP_T1_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t2_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T2_ACT_GPIO_Port, AMP_T2_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t3_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T3_ACT_GPIO_Port, AMP_T3_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t4_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T4_ACT_GPIO_Port, AMP_T4_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t5_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T5_ACT_GPIO_Port, AMP_T5_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t6_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T6_ACT_GPIO_Port, AMP_T6_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t7_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T7_ACT_GPIO_Port, AMP_T7_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t8_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T8_ACT_GPIO_Port, AMP_T8_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t9_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T9_ACT_GPIO_Port, AMP_T9_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_t10_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_T10_ACT_GPIO_Port, AMP_T10_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_amp_on_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(AMP_ON_ACT_GPIO_Port, AMP_ON_ACT_Pin);
  handle_amp_st_command(huart);
}
void handle_drv_on_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(DRV_ON_ACT_GPIO_Port, DRV_ON_ACT_Pin);
  handle_amp_st_command(huart);
}

void handle_osc_on_command (UART_HandleTypeDef *huart)
{
  HAL_GPIO_TogglePin(OSC_ON_GPIO_Port, OSC_ON_Pin);
  handle_amp_st_command(huart);
}

void handle_amp_st_command (UART_HandleTypeDef *huart)
{
  char buf[128];
  int len = snprintf(buf, sizeof(buf),
      "A1  A2  A3  A4  A5  A6  A7  A8  A9  A10 AON DON OSCON\r\n"
      " %d   %d   %d   %d   %d   %d   %d   %d   %d    %d   %d   %d     %d\r\n",
      HAL_GPIO_ReadPin(AMP_T1_ACT_GPIO_Port, AMP_T1_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T2_ACT_GPIO_Port, AMP_T2_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T3_ACT_GPIO_Port, AMP_T3_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T4_ACT_GPIO_Port, AMP_T4_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T5_ACT_GPIO_Port, AMP_T5_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T6_ACT_GPIO_Port, AMP_T6_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T7_ACT_GPIO_Port, AMP_T7_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T8_ACT_GPIO_Port, AMP_T8_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T9_ACT_GPIO_Port, AMP_T9_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_T10_ACT_GPIO_Port, AMP_T10_ACT_Pin),
      HAL_GPIO_ReadPin(AMP_ON_ACT_GPIO_Port, AMP_ON_ACT_Pin),
      HAL_GPIO_ReadPin(DRV_ON_ACT_GPIO_Port, DRV_ON_ACT_Pin),
      HAL_GPIO_ReadPin(OSC_ON_GPIO_Port, OSC_ON_Pin)
  );
  HAL_UART_Transmit(huart, (uint8_t*)buf, len, HAL_MAX_DELAY);
}

void handle_btn_1_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_1);
}
void handle_btn_8_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_8);
}
void handle_btn_9_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_9);
}
void handle_btn_left_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_LEFT);
}
void handle_btn_right_command (UART_HandleTypeDef *huart)
{
  send_btn_notify(BTN_RIGHT);
}


static void send_btn_notify (KeyCode_t btn)
{
//  player.priority = AUDIO_PRIORITY_LOW;

  LCDTaskEvent_t lcd_event = { .event = LCD_EVENT_BTN, .btn = {
      .button = btn,
      .pressed = true } };

  if (osMessageQueuePut(xLCDQueueHandle, &lcd_event, 0, 0) != osOK)
  {
    LOG_WARN("The lcd queue is full");
  }
}

static void notify_arming (bool val)
{
  AudioNotify_t audio_notify = { .event = AUDIO_ARMIG, .priority = AUDIO_PRIORITY_HIGH, .arming = val };
  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
}

static void notify_audio (AudioEvent_t event, AudioType_t type)
{
  AudioNotify_t audio_notify = { .event = event, .sample.type = type, .priority = AUDIO_PRIORITY_HIGH };

  if (osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10) != osOK)
  {
    LOG_WARN("The event from MOTOROLA cannot be handled. The audio queue is full");
  }
}
