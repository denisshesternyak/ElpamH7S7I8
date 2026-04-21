// system_status.c
#include "system_status.h"
#include <string.h>
//#include "lcd/lcd_widget_volume_indicator.h"

system_status_t system_status;

static char mode_to_char(system_mode_t mode)
{
    switch (mode) {
        case SYSTEM_MODE_ARMING:           return 'B';
        case SYSTEM_MODE_ALL_CLEAR_1:      return 'F';
        case SYSTEM_MODE_ALL_CLEAR_2:      return 'G';
        case SYSTEM_MODE_ALARM_WAIL:       return 'H';
        case SYSTEM_MODE_CHEMICAL:         return 'L';
        case SYSTEM_MODE_CANCEL_IMMEDIATE: return 'A';
        case SYSTEM_MODE_CANCEL_DELAYED:   return 'C';
        case SYSTEM_MODE_QUIET_TEST:       return 'O';
        case SYSTEM_MODE_FUTURE_SIREN_1:   return 'I';
        case SYSTEM_MODE_FUTURE_SIREN_2:   return 'J';
        case SYSTEM_MODE_FUTURE_SIREN_3:   return 'K';
        case SYSTEM_MODE_REPORT:           return 'D';
        case SYSTEM_MODE_VOICE:            return 'N';
        case SYSTEM_MODE_RESET:            return 'E';
        case SYSTEM_MODE_IDLE:             return '0';
        default:                           return ' ';
    }
}

void system_status_init(void)
{
    memset(&system_status, 0, sizeof(system_status));
    system_status.mode = SYSTEM_MODE_IDLE;

    for (int i = 0; i < 10; i++) {
        system_status.amplifier_driver[i] = true;
    }

    system_status.door_sensor = false;
    system_status.flood_sensor = false;
    system_status.ac_voltage = true;
    system_status.battery_voltage = true;
    system_status.charger_unit = true;
}

void system_status_reset(void)
{
    char saved_mode_char = system_status_get_mode_char();
    bool saved_charger_unit = system_status.charger_unit;

    memset(&system_status, 0, sizeof(system_status));

    if (saved_mode_char == 'A') {
    	system_status_set_mode(SYSTEM_MODE_STANDBY);
    } else if (saved_mode_char == 'B') {
    	system_status_set_mode(SYSTEM_MODE_ARMING);
    } else if (saved_mode_char == 'E') {
    	system_status_set_mode(SYSTEM_MODE_RESET);
    } else {
        system_status_set_mode(SYSTEM_MODE_IDLE);
    }

    system_status.charger_unit = saved_charger_unit;

    system_status.operating_current = false;
    system_status.battery_voltage = false;
    system_status.ac_voltage = false;
    system_status.flood_sensor = false;
    system_status.door_sensor = false;

    system_status.max_volume = true;
    system_status.main_tone = true;
    system_status.secondary_tone = true;
    system_status.not_used = false;

    system_status.door_sensor = true;

    //system_set_volume(DEF_VALUE_VOLUME);
}

char system_status_get_mode_char(void)
{
    return mode_to_char(system_status.mode);
}

void system_status_set_mode(system_mode_t mode)
{
    system_status.mode = mode;
}
