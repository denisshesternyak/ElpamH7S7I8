#include "rs232.h"
#include <string.h>
#include <stdbool.h>

//#include "audio_types.h"
#include "app_freertos.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "defines.h"

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

static rs232_cmd_handler_t handle_amp_t1 = NULL;
static rs232_cmd_handler_t handle_amp_t2 = NULL;
static rs232_cmd_handler_t handle_amp_t3 = NULL;
static rs232_cmd_handler_t handle_amp_t4 = NULL;
static rs232_cmd_handler_t handle_amp_t5 = NULL;
static rs232_cmd_handler_t handle_amp_t6 = NULL;
static rs232_cmd_handler_t handle_amp_t7 = NULL;
static rs232_cmd_handler_t handle_amp_t8 = NULL;
static rs232_cmd_handler_t handle_amp_t9 = NULL;
static rs232_cmd_handler_t handle_amp_t10 = NULL;
static rs232_cmd_handler_t handle_amp_on = NULL;
static rs232_cmd_handler_t handle_drv_on = NULL;
static rs232_cmd_handler_t handle_amp_st = NULL;
static rs232_cmd_handler_t handle_osc_on = NULL;


static rs232_cmd_handler_t handle_btn_1 = NULL;
static rs232_cmd_handler_t handle_btn_8 = NULL;
static rs232_cmd_handler_t handle_btn_9 = NULL;
static rs232_cmd_handler_t handle_btn_left = NULL;
static rs232_cmd_handler_t handle_btn_right = NULL;

typedef struct {
    uint32_t last_rx_time;
    char rx_buffer[CMD_LENGTH + 1];
    uint8_t rx_count;
    uint8_t rx_byte;
    uint8_t reception_active;
    bool sent_unknown;
} UartContext_t;

static UartContext_t rs232a_ctx = {0};
static UartContext_t rs232b_ctx = {0};

static void rs232a_rx_complete_callback (UART_HandleTypeDef *huart);
static void call_or_default (UART_HandleTypeDef *huart, rs232_cmd_handler_t h);
static void call_or_default_unknown (UART_HandleTypeDef *huart);
static void process_command (UART_HandleTypeDef *huart, char *cmd);

static uint16_t volume_value;

static void process_uart_rx (UART_HandleTypeDef *huart,
			     UartContext_t *ctx)
{
  uint32_t now = osKernelGetTickCount();

  if ((now - ctx->last_rx_time) > ACTIVATION_CMD_TIMEOUT)
  {
    ctx->rx_count = 0;
    ctx->sent_unknown = false;
  }
  ctx->last_rx_time = now;

  if (ctx->reception_active && (ctx->rx_byte >= 32 && ctx->rx_byte <= 126) && ctx->rx_count < CMD_LENGTH)
  {
    if (ctx->rx_count == 0 && ctx->rx_byte != '*')
    {
      if (!ctx->sent_unknown)
      {
	ctx->sent_unknown = true;
	UartEvent_t event = UART_EVENT_UNKNOWN;
	osMessageQueuePut(xUartQueueHandle, &event, 0U, 0U);
      }
    }
    else
    {
      ctx->rx_buffer[ctx->rx_count++] = ctx->rx_byte;
      if (ctx->rx_count == CMD_LENGTH)
      {
	ctx->rx_buffer[CMD_LENGTH] = '\0';
	process_command(huart, ctx->rx_buffer);
	ctx->rx_count = 0;
      }
    }
  }
  else
  {
    ctx->rx_count = 0;
  }
}

static void rs232a_rx_complete_callback (UART_HandleTypeDef *huart)
{
  process_uart_rx(huart, &rs232a_ctx);
  HAL_UART_Receive_IT(huart, &rs232a_ctx.rx_byte, 1);
}

static void rs232b_rx_complete_callback (UART_HandleTypeDef *huart)
{
  process_uart_rx(huart, &rs232b_ctx);
  HAL_UART_Receive_IT(huart, &rs232b_ctx.rx_byte, 1);
}

void rs232_init ()
{
  usart_register_rx_callback(&huart7, rs232a_rx_complete_callback);
  usart_register_rx_callback(&huart4, rs232b_rx_complete_callback);

  HAL_UART_Receive_IT(&huart7, &rs232a_ctx.rx_byte, 1);
  HAL_UART_Receive_IT(&huart4, &rs232b_ctx.rx_byte, 1);

  rs232a_ctx.reception_active = 1;
  rs232b_ctx.reception_active = 1;
}

void rs232_process (UartMessage_t *msg)
{
  switch (msg->event)
  {
    case UART_EVENT_ARM:
      call_or_default(msg->huart, handler_arm);
      break;
    case UART_EVENT_A_CLR1:
      call_or_default(msg->huart, handler_all_clear_1);
      break;
    case UART_EVENT_A_CLR2:
      call_or_default(msg->huart, handler_all_clear_2);
      break;
    case UART_EVENT_I_WAIL:
      call_or_default(msg->huart, handler_alarm);
      break;
    case UART_EVENT_CHEM_A:
      call_or_default(msg->huart, handler_chemical);
      break;
    case UART_EVENT_DISARM:
      call_or_default(msg->huart, handler_disarm);
      break;
    case UART_EVENT_CANCEL:
      call_or_default(msg->huart, handler_cancel);
      break;
    case UART_EVENT_Q_TEST:
      call_or_default(msg->huart, handler_quiet_test);
      break;
    case UART_EVENT_WAIL:
      call_or_default(msg->huart, handler_reserve_1);
      break;
    case UART_EVENT_P_WAIL:
      call_or_default(msg->huart, handler_reserve_2);
      break;
    case UART_EVENT_YELP:
      call_or_default(msg->huart, handler_reserve_3);
      break;
    case UART_EVENT_REPORT:
      call_or_default(msg->huart, handler_report);
      break;
    case UART_EVENT_VOICE:
      call_or_default(msg->huart, handler_remote_pa);
      break;
    case UART_EVENT_RESET:
      call_or_default(msg->huart, handler_reset);
      break;
    case UART_EVENT_VOL_UP:
      handler_volume_up(volume_value);
      break;
    case UART_EVENT_VOL_DOWN:
      handler_volume_down(volume_value - 900 + 1);
      break;
    case UART_EVENT_UNKNOWN:
      call_or_default_unknown(msg->huart);
      break;

    case UART_EVENT_ENTER_BTN:
      call_or_default(msg->huart, handler_enter);
      break;
    case UART_EVENT_UP_BTN:
      call_or_default(msg->huart, handler_up);
      break;
    case UART_EVENT_DOWEN_BTN:
      call_or_default(msg->huart, handler_down);
      break;
    case UART_EVENT_ESC_BTN:
      call_or_default(msg->huart, handler_esc);
      break;
    case UART_EVENT_CANCEL_BTN:
      call_or_default(msg->huart, handler_cnlbtn);
      break;
    case UART_EVENT_TEST_BTN:
      call_or_default(msg->huart, handler_test);
      break;
    case UART_EVENT_ANNOUNCEMENT_BTN:
      call_or_default(msg->huart, handler_announc);
      break;
    case UART_EVENT_MESSAGE_BTN:
      call_or_default(msg->huart, handler_message);
      break;
    case UART_EVENT_ALARM_BTN:
      call_or_default(msg->huart, handler_almbtn);
      break;
    case UART_EVENT_ARM_BTN:
      call_or_default(msg->huart, handle_armbtn);
      break;

    case UART_EVENT_AMP1_BTN:
      call_or_default(msg->huart, handle_amp_t1);
      break;
    case UART_EVENT_AMP2_BTN:
      call_or_default(msg->huart, handle_amp_t2);
      break;
    case UART_EVENT_AMP3_BTN:
      call_or_default(msg->huart, handle_amp_t3);
      break;
    case UART_EVENT_AMP4_BTN:
      call_or_default(msg->huart, handle_amp_t4);
      break;
    case UART_EVENT_AMP5_BTN:
      call_or_default(msg->huart, handle_amp_t5);
      break;
    case UART_EVENT_AMP6_BTN:
      call_or_default(msg->huart, handle_amp_t6);
      break;
    case UART_EVENT_AMP7_BTN:
      call_or_default(msg->huart, handle_amp_t7);
      break;
    case UART_EVENT_AMP8_BTN:
      call_or_default(msg->huart, handle_amp_t8);
      break;
    case UART_EVENT_AMP9_BTN:
      call_or_default(msg->huart, handle_amp_t9);
      break;
    case UART_EVENT_AMP10_BTN:
      call_or_default(msg->huart, handle_amp_t10);
      break;
    case UART_EVENT_AMP_ON_BTN:
      call_or_default(msg->huart, handle_amp_on);
      break;
    case UART_EVENT_DRV_ON_BTN:
      call_or_default(msg->huart, handle_drv_on);
      break;
    case UART_EVENT_AMP_ST_BTN:
      call_or_default(msg->huart, handle_amp_st);
      break;
    case UART_EVENT_OSC_ON_BTN:
      call_or_default(msg->huart, handle_osc_on);
      break;

    case UART_EVENT_BTN_1:
      call_or_default(msg->huart, handle_btn_1);
      break;
    case UART_EVENT_BTN_8:
      call_or_default(msg->huart, handle_btn_8);
      break;
    case UART_EVENT_BTN_9:
      call_or_default(msg->huart, handle_btn_9);
      break;
    case UART_EVENT_BTN_LEFT:
      call_or_default(msg->huart, handle_btn_left);
      break;
    case UART_EVENT_BTN_RIGHT:
      call_or_default(msg->huart, handle_btn_right);
      break;
  }
}

static void process_command (UART_HandleTypeDef *huart, char *cmd)
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
  else if (strncmp(cmd, "*AMP_T1", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP1_BTN;
  }
  else if (strncmp(cmd, "*AMP_T2", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP2_BTN;
  }
  else if (strncmp(cmd, "*AMP_T3", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP3_BTN;
  }
  else if (strncmp(cmd, "*AMP_T4", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP4_BTN;
  }
  else if (strncmp(cmd, "*AMP_T5", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP5_BTN;
  }
  else if (strncmp(cmd, "*AMP_T6", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP6_BTN;
  }
  else if (strncmp(cmd, "*AMP_T7", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP7_BTN;
  }
  else if (strncmp(cmd, "*AMP_T8", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP8_BTN;
  }
  else if (strncmp(cmd, "*AMP_T9", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP9_BTN;
  }
  else if (strncmp(cmd, "*AMPT10", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP10_BTN;
  }
  else if (strncmp(cmd, "*AMP_ON", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP_ON_BTN;
  }
  else if (strncmp(cmd, "*DRV_ON", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_DRV_ON_BTN;
  }
  else if (strncmp(cmd, "*OSC_ON", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_OSC_ON_BTN;
  }
  else if (strncmp(cmd, "*AMP_ST", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_AMP_ST_BTN;
  }
  else if (strncmp(cmd, "*BTN__1", CMD_LENGTH) == 0)
    {
      event = UART_EVENT_BTN_1;
    }
  else if (strncmp(cmd, "*BTN__8", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_BTN_8;
  }
  else if (strncmp(cmd, "*BTN__9", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_BTN_9;
  }
  else if (strncmp(cmd, "*BTNLFT", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_BTN_LEFT;
  }
  else if (strncmp(cmd, "*BTNRGT", CMD_LENGTH) == 0)
  {
    event = UART_EVENT_BTN_RIGHT;
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

  UartMessage_t msg = { .huart = huart, .event = event};
  osMessageQueuePut(xUartQueueHandle, &msg, 0U, 0U);
}

static void call_or_default (UART_HandleTypeDef *huart, rs232_cmd_handler_t h)
{
  if (h)
    h(huart);
  else
    HAL_UART_Transmit(huart, (uint8_t*) "NO HANDLER\r\n", 12, HAL_MAX_DELAY);
}

static void call_or_default_unknown (UART_HandleTypeDef *huart)
{
  if (handler_unknown)
    handler_unknown(huart);
  else
    HAL_UART_Transmit(huart, (uint8_t*) "ERR:UNKNOWN\r\n", 13, HAL_MAX_DELAY);
}

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
void rs232_register_amp_t1 (rs232_cmd_handler_t h)
{
  handle_amp_t1 = h;
}
void rs232_register_amp_t2 (rs232_cmd_handler_t h)
{
  handle_amp_t2 = h;
}
void rs232_register_amp_t3 (rs232_cmd_handler_t h)
{
  handle_amp_t3 = h;
}
void rs232_register_amp_t4 (rs232_cmd_handler_t h)
{
  handle_amp_t4 = h;
}
void rs232_register_amp_t5 (rs232_cmd_handler_t h)
{
  handle_amp_t5 = h;
}
void rs232_register_amp_t6 (rs232_cmd_handler_t h)
{
  handle_amp_t6 = h;
}
void rs232_register_amp_t7 (rs232_cmd_handler_t h)
{
  handle_amp_t7 = h;
}
void rs232_register_amp_t8 (rs232_cmd_handler_t h)
{
  handle_amp_t8 = h;
}
void rs232_register_amp_t9 (rs232_cmd_handler_t h)
{
  handle_amp_t9 = h;
}
void rs232_register_amp_t10 (rs232_cmd_handler_t h)
{
  handle_amp_t10 = h;
}
void rs232_register_amp_on (rs232_cmd_handler_t h)
{
  handle_amp_on = h;
}
void rs232_register_drv_on (rs232_cmd_handler_t h)
{
  handle_drv_on = h;
}
void rs232_register_amp_st (rs232_cmd_handler_t h)
{
  handle_amp_st = h;
}
void rs232_register_osc_on (rs232_cmd_handler_t h)
{
  handle_osc_on = h;
}

void rs232_register_btn_1 (rs232_cmd_handler_t h)
{
  handle_btn_1 = h;
}
void rs232_register_btn_8 (rs232_cmd_handler_t h)
{
  handle_btn_8 = h;
}
void rs232_register_btn_9 (rs232_cmd_handler_t h)
{
  handle_btn_9 = h;
}
void rs232_register_btn_left (rs232_cmd_handler_t h)
{
  handle_btn_left = h;
}
void rs232_register_btn_right (rs232_cmd_handler_t h)
{
  handle_btn_right = h;
}
