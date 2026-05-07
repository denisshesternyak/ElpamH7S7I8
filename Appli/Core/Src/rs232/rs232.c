#include "rs232.h"
#include <string.h>
#include <stdbool.h>

//#include "audio_types.h"
#include "app_freertos.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "defines.h"

#define RS232_HANDLER	&huart4

static uint8_t rx_byte;
static char rx_buffer[8];
static uint8_t rx_count = 0;
static uint8_t reception_active = 0;

static rs232_cmd_handler_t handler_arm = NULL;
static rs232_cmd_handler_t handler_all_clear_1 = NULL;
static rs232_cmd_handler_t handler_all_clear_2 = NULL;
static rs232_cmd_handler_t handler_alarm = NULL;
static rs232_cmd_handler_t handler_chemical = NULL;
static rs232_cmd_handler_t handler_disarm = NULL;
static rs232_cmd_handler_t handler_cancel = NULL;
static rs232_cmd_handler_t handler_quiet_test = NULL;
static rs232_cmd_handler_t handler_reserve_1 = NULL;
static rs232_cmd_handler_t handler_reserve_2 = NULL;
static rs232_cmd_handler_t handler_reserve_3 = NULL;
static rs232_cmd_handler_t handler_report = NULL;
static rs232_cmd_handler_t handler_remote_pa = NULL;
static rs232_cmd_handler_t handler_reset = NULL;
static rs232_volume_handler_t handler_volume_up = NULL;
static rs232_volume_handler_t handler_volume_down = NULL;
static rs232_cmd_handler_t handler_unknown = NULL;

static rs232_cmd_handler_t handler_enter = NULL;
static rs232_cmd_handler_t handler_up = NULL;
static rs232_cmd_handler_t handler_down = NULL;
static rs232_cmd_handler_t handler_esc = NULL;
static rs232_cmd_handler_t handler_cnlbtn = NULL;
static rs232_cmd_handler_t handler_test = NULL;
static rs232_cmd_handler_t handler_announc = NULL;
static rs232_cmd_handler_t handler_message = NULL;
static rs232_cmd_handler_t handler_almbtn = NULL;
static rs232_cmd_handler_t handle_armbtn = NULL;

static void rs232a_rx_complete_callback (UART_HandleTypeDef *huart);
static void call_or_default (rs232_cmd_handler_t h);
static void call_or_default_unknown (void);
static void process_command (char *cmd);

static uint16_t volume_value;
static bool sent_unknown;

static void rs232a_rx_complete_callback (UART_HandleTypeDef *huart)
{
  static uint32_t last_rx_time = 0;
  uint32_t now = osKernelGetTickCount();

  if ((now - last_rx_time) > ACTIVATION_CMD_TIMEOUT)
  {
    rx_count = 0;
    sent_unknown = false;
  }

  last_rx_time = now;

  if (reception_active && (rx_byte >= 32 && rx_byte <= 126) && rx_count < CMD_LENGTH)
  {
    if (rx_count == 0 && rx_byte != '*')
    {
      if (!sent_unknown)
      {
	sent_unknown = true;
	UartEvent_t event = UART_EVENT_UNKNOWN;
	osMessageQueuePut(xUartQueueHandle, &event, 0U, 0U);
      }
    }
    else
    {
      rx_buffer[rx_count++] = rx_byte;
      if (rx_count == CMD_LENGTH)
      {
	rx_buffer[CMD_LENGTH] = '\0';
	process_command(rx_buffer);
	rx_count = 0;
      }
    }
  }
  else
  {
    rx_count = 0;
  }

  HAL_UART_Receive_IT(RS232_HANDLER, &rx_byte, 1);
}

void rs232_init ()
{
  usart_register_rx_callback(RS232_HANDLER, rs232a_rx_complete_callback);

  HAL_UART_Receive_IT(RS232_HANDLER, &rx_byte, 1);
  reception_active = 1;
}

void rs232_process (UartEvent_t event)
{
  switch (event)
  {
    case UART_EVENT_ARM:
      call_or_default(handler_arm);
      break;
    case UART_EVENT_A_CLR1:
      call_or_default(handler_all_clear_1);
      break;
    case UART_EVENT_A_CLR2:
      call_or_default(handler_all_clear_2);
      break;
    case UART_EVENT_I_WAIL:
      call_or_default(handler_alarm);
      break;
    case UART_EVENT_CHEM_A:
      call_or_default(handler_chemical);
      break;
    case UART_EVENT_DISARM:
      call_or_default(handler_disarm);
      break;
    case UART_EVENT_CANCEL:
      call_or_default(handler_cancel);
      break;
    case UART_EVENT_Q_TEST:
      call_or_default(handler_quiet_test);
      break;
    case UART_EVENT_WAIL:
      call_or_default(handler_reserve_1);
      break;
    case UART_EVENT_P_WAIL:
      call_or_default(handler_reserve_2);
      break;
    case UART_EVENT_YELP:
      call_or_default(handler_reserve_3);
      break;
    case UART_EVENT_REPORT:
      call_or_default(handler_report);
      break;
    case UART_EVENT_VOICE:
      call_or_default(handler_remote_pa);
      break;
    case UART_EVENT_RESET:
      call_or_default(handler_reset);
      break;
    case UART_EVENT_VOL_UP:
      handler_volume_up(volume_value);
      break;
    case UART_EVENT_VOL_DOWN:
      handler_volume_down(volume_value - 900 + 1);
      break;
    case UART_EVENT_UNKNOWN:
      call_or_default_unknown();
      break;

    case UART_EVENT_ENTER_BTN:
      call_or_default(handler_enter);
      break;
    case UART_EVENT_UP_BTN:
      call_or_default(handler_up);
      break;
    case UART_EVENT_DOWEN_BTN:
      call_or_default(handler_down);
      break;
    case UART_EVENT_ESC_BTN:
      call_or_default(handler_esc);
      break;
    case UART_EVENT_CANCEL_BTN:
      call_or_default(handler_cnlbtn);
      break;
    case UART_EVENT_TEST_BTN:
      call_or_default(handler_test);
      break;
    case UART_EVENT_ANNOUNCEMENT_BTN:
      call_or_default(handler_announc);
      break;
    case UART_EVENT_MESSAGE_BTN:
      call_or_default(handler_message);
      break;
    case UART_EVENT_ALARM_BTN:
      call_or_default(handler_almbtn);
      break;
    case UART_EVENT_ARM_BTN:
      call_or_default(handle_armbtn);
      break;
  }
}

static void process_command (char *cmd)
{
  UartEvent_t event;

  if (strncmp(cmd, "*_ARM__", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_ARM;
  }
  else if (strncmp(cmd, "*A_CLR1", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_A_CLR1;
  }
  else if (strncmp(cmd, "*A_CLR2", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_A_CLR2;
  }
  else if (strncmp(cmd, "*I_WAIL", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_I_WAIL;
  }
  else if (strncmp(cmd, "*CHEM_A", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_CHEM_A;
  }
  else if (strncmp(cmd, "*DISARM", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_DISARM;
  }
  else if (strncmp(cmd, "*CANCEL", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_CANCEL;
  }
  else if (strncmp(cmd, "*Q_TEST", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_Q_TEST;
  }
  else if (strncmp(cmd, "*_WAIL_", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_WAIL;
  }
  else if (strncmp(cmd, "*P_WAIL", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_P_WAIL;
  }
  else if (strncmp(cmd, "*YELP__", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_YELP;
  }
  else if (strncmp(cmd, "*REPORT", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_REPORT;
  }
  else if (strncmp(cmd, "*_VOICE", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_VOICE;
  }
  else if (strncmp(cmd, "*RESET_", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_RESET;
  }

  else if (strncmp(cmd, "*ENTER_", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_ENTER_BTN;
  }
  else if (strncmp(cmd, "*UP____", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_UP_BTN;
  }
  else if (strncmp(cmd, "*DOWN__", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_DOWEN_BTN;
  }
  else if (strncmp(cmd, "*ESC___", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_ESC_BTN;
  }
  else if (strncmp(cmd, "*CNLBTN", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_CANCEL_BTN;
  }
  else if (strncmp(cmd, "*TEST__", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_TEST_BTN;
  }
  else if (strncmp(cmd, "*ANNOUN", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_ANNOUNCEMENT_BTN;
  }
  else if (strncmp(cmd, "*MESSAG", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_MESSAGE_BTN;
  }
  else if (strncmp(cmd, "*ALMBTN", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_ALARM_BTN;
  }
  else if (strncmp(cmd, "*ARMBTN", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_ARM_BTN;
  }

  else if (strncmp(cmd, "*VOL", 4) == 0)
  {
    int d1 = (cmd[4] == ' ') ? 0 : (cmd[4] - '0');
    int d2 = (cmd[5] - '0');
    int d3 = (cmd[6] - '0');

    if ((cmd[4] == ' ' || (cmd[4] >= '0' && cmd[4] <= '9')) && (cmd[5] >= '0' && cmd[5] <= '9') && (cmd[6] >= '0' && cmd[6] <= '9'))
    {
      volume_value = d1 * 100 + d2 * 10 + d3;
      if (volume_value >= MIN_VOLUME && volume_value <= MAX_VOLUME && handler_volume_up)
	event = UART_EVENT_VOL_UP;
      else if (volume_value >= 900 && volume_value <= 999 && handler_volume_down)
	event = UART_EVENT_VOL_DOWN;
      else
	event = UART_EVENT_UNKNOWN;
    }
    else
      event = UART_EVENT_UNKNOWN;
  }
  else
    event = UART_EVENT_UNKNOWN;

  osMessageQueuePut(xUartQueueHandle, &event, 0U, 0U);
}

static void call_or_default (rs232_cmd_handler_t h)
{
  if (h)
    h(RS232_HANDLER);
  else
    HAL_UART_Transmit(RS232_HANDLER, (uint8_t*) "NO HANDLER\r\n", 12, HAL_MAX_DELAY);
}

static void call_or_default_unknown (void)
{
  if (handler_unknown)
    handler_unknown(RS232_HANDLER);
  else
    HAL_UART_Transmit(RS232_HANDLER, (uint8_t*) "ERR:UNKNOWN\r\n", 13, HAL_MAX_DELAY);
  rx_count = 0;
}

// ————————————————————————
//// Registration functions
// ————————————————————————

void rs232_register_arm (rs232_cmd_handler_t h)
{
  handler_arm = h;
}
void rs232_register_all_clear_1 (rs232_cmd_handler_t h)
{
  handler_all_clear_1 = h;
}
void rs232_register_all_clear_2 (rs232_cmd_handler_t h)
{
  handler_all_clear_2 = h;
}
void rs232_register_alarm (rs232_cmd_handler_t h)
{
  handler_alarm = h;
}
void rs232_register_chemical (rs232_cmd_handler_t h)
{
  handler_chemical = h;
}
void rs232_register_disarm (rs232_cmd_handler_t h)
{
  handler_disarm = h;
}
void rs232_register_cancel (rs232_cmd_handler_t h)
{
  handler_cancel = h;
}
void rs232_register_quiet_test (rs232_cmd_handler_t h)
{
  handler_quiet_test = h;
}
void rs232_register_reserve_1 (rs232_cmd_handler_t h)
{
  handler_reserve_1 = h;
}
void rs232_register_reserve_2 (rs232_cmd_handler_t h)
{
  handler_reserve_2 = h;
}
void rs232_register_reserve_3 (rs232_cmd_handler_t h)
{
  handler_reserve_3 = h;
}
void rs232_register_report (rs232_cmd_handler_t h)
{
  handler_report = h;
}
void rs232_register_remote_pa (rs232_cmd_handler_t h)
{
  handler_remote_pa = h;
}
void rs232_register_reset (rs232_cmd_handler_t h)
{
  handler_reset = h;
}
void rs232_register_volume_up (rs232_volume_handler_t h)
{
  handler_volume_up = h;
}
void rs232_register_volume_down (rs232_volume_handler_t h)
{
  handler_volume_down = h;
}
void rs232_register_unknown (rs232_cmd_handler_t h)
{
  handler_unknown = h;
}

void rs232_register_enter (rs232_cmd_handler_t h)
{
  handler_enter = h;
}
void rs232_register_up (rs232_cmd_handler_t h)
{
  handler_up = h;
}
void rs232_register_down (rs232_cmd_handler_t h)
{
  handler_down = h;
}
void rs232_register_esc (rs232_cmd_handler_t h)
{
  handler_esc = h;
}
void rs232_register_cnlbtn (rs232_cmd_handler_t h)
{
  handler_cnlbtn = h;
}
void rs232_register_test (rs232_cmd_handler_t h)
{
  handler_test = h;
}
void rs232_register_announc (rs232_cmd_handler_t h)
{
  handler_announc = h;
}
void rs232_register_message (rs232_cmd_handler_t h)
{
  handler_message = h;
}
void rs232_register_almbtn (rs232_cmd_handler_t h)
{
  handler_almbtn = h;
}
void rs232_register_armbtn (rs232_cmd_handler_t h)
{
  handle_armbtn = h;
}

