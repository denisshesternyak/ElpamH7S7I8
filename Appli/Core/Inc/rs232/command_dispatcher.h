#ifndef INC_RS232_COMMAND_DISPATCHER_H_
#define INC_RS232_COMMAND_DISPATCHER_H_

#include <stdint.h>
#include "usart.h"

void handle_arm (UART_HandleTypeDef *huart);
void handle_all_clear_1 (UART_HandleTypeDef *huart);
void handle_all_clear_2 (UART_HandleTypeDef *huart);
void handle_alarm (UART_HandleTypeDef *huart);
void handle_chemical (UART_HandleTypeDef *huart);
void handle_disarm (UART_HandleTypeDef *huart);
void handle_cancel (UART_HandleTypeDef *huart);
void handle_quiet_test (UART_HandleTypeDef *huart);
void handle_reserve_1 (UART_HandleTypeDef *huart);
void handle_reserve_2 (UART_HandleTypeDef *huart);
void handle_reserve_3 (UART_HandleTypeDef *huart);
void handle_remote_pa (UART_HandleTypeDef *huart);
void handle_reset (UART_HandleTypeDef *huart);

void volume_up_handler (int step);
void volume_down_handler (int step);

void system_fill_report (UART_HandleTypeDef *huart);

typedef void (* outputs_handler_t) (void);
void prepare_outputs_on(outputs_handler_t h);
void prepare_outputs_off(outputs_handler_t h);

void handle_unknown_command (UART_HandleTypeDef *huart);

void handle_enter_command (UART_HandleTypeDef *huart);
void handle_up_command (UART_HandleTypeDef *huart);
void handle_down_command (UART_HandleTypeDef *huart);
void handle_esc_command (UART_HandleTypeDef *huart);
void handle_cancel_command (UART_HandleTypeDef *huart);
void handle_test_command (UART_HandleTypeDef *huart);
void handle_announc_command (UART_HandleTypeDef *huart);
void handle_message_command (UART_HandleTypeDef *huart);
void handle_alarm_command (UART_HandleTypeDef *huart);
void handle_arm_command (UART_HandleTypeDef *huart);

void handle_amp_t1_command (UART_HandleTypeDef *huart);
void handle_amp_t2_command (UART_HandleTypeDef *huart);
void handle_amp_t3_command (UART_HandleTypeDef *huart);
void handle_amp_t4_command (UART_HandleTypeDef *huart);
void handle_amp_t5_command (UART_HandleTypeDef *huart);
void handle_amp_t6_command (UART_HandleTypeDef *huart);
void handle_amp_t7_command (UART_HandleTypeDef *huart);
void handle_amp_t8_command (UART_HandleTypeDef *huart);
void handle_amp_t9_command (UART_HandleTypeDef *huart);
void handle_amp_t10_command (UART_HandleTypeDef *huart);
void handle_amp_on_command (UART_HandleTypeDef *huart);
void handle_drv_on_command (UART_HandleTypeDef *huart);
void handle_amp_st_command (UART_HandleTypeDef *huart);
void handle_osc_on_command (UART_HandleTypeDef *huart);

void handle_btn_1_command (UART_HandleTypeDef *huart);
void handle_btn_8_command (UART_HandleTypeDef *huart);
void handle_btn_9_command (UART_HandleTypeDef *huart);
void handle_btn_left_command (UART_HandleTypeDef *huart);
void handle_btn_right_command (UART_HandleTypeDef *huart);

#endif /* INC_RS232_COMMAND_DISPATCHER_H_ */
