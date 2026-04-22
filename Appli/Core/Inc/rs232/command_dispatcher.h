#ifndef INC_RS232_COMMAND_DISPATCHER_H_
#define INC_RS232_COMMAND_DISPATCHER_H_

#include <stdint.h>

void handle_arm (void);
void handle_all_clear_1 (void);
void handle_all_clear_2 (void);
void handle_alarm (void);
void handle_chemical (void);
void handle_disarm (void);
void handle_cancel (void);
void handle_quiet_test (void);
void handle_reserve_1 (void);
void handle_reserve_2 (void);
void handle_reserve_3 (void);
void handle_remote_pa (void);
void handle_reset (void);

void volume_up_handler (int step);
void volume_down_handler (int step);

void system_fill_report (void);

void handle_unknown_command (void);

void handle_enter_command (void);
void handle_up_command (void);
void handle_down_command (void);
void handle_esc_command (void);
void handle_cancel_command (void);
void handle_test_command (void);
void handle_announc_command (void);
void handle_message_command (void);
void handle_alarm_command (void);
void handle_arm_command (void);

#endif /* INC_RS232_COMMAND_DISPATCHER_H_ */
