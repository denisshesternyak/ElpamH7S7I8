#ifndef INC_RS232_RS232_H_
#define INC_RS232_RS232_H_

#include <stdint.h>
#include "stm32h7rsxx_hal.h"

#define CMD_LENGTH 			7	// Command length — 7 characters
#define ACTIVATION_CMD_TIMEOUT  	10

typedef enum
{
  UART_EVENT_ARM,
  UART_EVENT_A_CLR1,
  UART_EVENT_A_CLR2,
  UART_EVENT_I_WAIL,
  UART_EVENT_CHEM_A,
  UART_EVENT_DISARM,
  UART_EVENT_CANCEL,
  UART_EVENT_Q_TEST,
  UART_EVENT_WAIL,
  UART_EVENT_P_WAIL,
  UART_EVENT_YELP,
  UART_EVENT_REPORT,
  UART_EVENT_VOICE,
  UART_EVENT_RESET,
  UART_EVENT_VOL_UP,
  UART_EVENT_VOL_DOWN,
  UART_EVENT_UNKNOWN,

  UART_EVENT_ENTER_BTN,
  UART_EVENT_UP_BTN,
  UART_EVENT_DOWEN_BTN,
  UART_EVENT_ESC_BTN,
  UART_EVENT_CANCEL_BTN,
  UART_EVENT_TEST_BTN,
  UART_EVENT_ANNOUNCEMENT_BTN,
  UART_EVENT_MESSAGE_BTN,
  UART_EVENT_ALARM_BTN,
  UART_EVENT_ARM_BTN
} UartEvent_t;

typedef void (*rs232_cmd_handler_t) (void);
typedef void (*rs232_volume_handler_t) (int step);

void rs232_init (UART_HandleTypeDef *huart);
void rs232_process (UartEvent_t event);

// Functions for registering handlers
void rs232_register_arm (rs232_cmd_handler_t handler);
void rs232_register_all_clear_1 (rs232_cmd_handler_t handler);
void rs232_register_all_clear_2 (rs232_cmd_handler_t handler);
void rs232_register_alarm (rs232_cmd_handler_t handler);
void rs232_register_chemical (rs232_cmd_handler_t handler);
void rs232_register_disarm (rs232_cmd_handler_t handler);
void rs232_register_cancel (rs232_cmd_handler_t handler);
void rs232_register_quiet_test (rs232_cmd_handler_t handler);
void rs232_register_reserve_1 (rs232_cmd_handler_t handler);
void rs232_register_reserve_2 (rs232_cmd_handler_t handler);
void rs232_register_reserve_3 (rs232_cmd_handler_t handler);
void rs232_register_report (rs232_cmd_handler_t handler);
void rs232_register_remote_pa (rs232_cmd_handler_t handler);
void rs232_register_reset (rs232_cmd_handler_t handler);
void rs232_register_volume_up (rs232_volume_handler_t handler);
void rs232_register_volume_down (rs232_volume_handler_t handler);
void rs232_register_unknown (rs232_cmd_handler_t handler);

void rs232_register_enter (rs232_cmd_handler_t handler);
void rs232_register_up (rs232_cmd_handler_t handler);
void rs232_register_down (rs232_cmd_handler_t handler);
void rs232_register_esc (rs232_cmd_handler_t handler);
void rs232_register_cnlbtn (rs232_cmd_handler_t handler);
void rs232_register_test (rs232_cmd_handler_t handler);
void rs232_register_announc (rs232_cmd_handler_t handler);
void rs232_register_message (rs232_cmd_handler_t handler);
void rs232_register_almbtn (rs232_cmd_handler_t handler);
void rs232_register_armbtn (rs232_cmd_handler_t handler);

#endif /* INC_RS232_RS232_H_ */
