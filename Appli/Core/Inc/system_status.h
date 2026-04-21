// system_status.h
#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

#include <stdint.h>
#include <stdbool.h>


// List of system modes
typedef enum {
    SYSTEM_MODE_ARMING,           // 'B' — Arming
    SYSTEM_MODE_ALL_CLEAR_1,      // 'F' — All Clear 1
    SYSTEM_MODE_ALL_CLEAR_2,      // 'G' — All Clear 2
    SYSTEM_MODE_ALARM_WAIL,       // 'H' — Alarm Operation (Wail)
    SYSTEM_MODE_CHEMICAL,         // 'L' — Chemical
    SYSTEM_MODE_CANCEL_IMMEDIATE, // 'A' — Cancellation siren - Immediate
    SYSTEM_MODE_CANCEL_DELAYED,   // 'C' — Cancellation siren - Delayed
    SYSTEM_MODE_QUIET_TEST,       // 'O' — Quiet Test
    SYSTEM_MODE_FUTURE_SIREN_1,   // 'I' — Future siren type #1
    SYSTEM_MODE_FUTURE_SIREN_2,   // 'J' — Future siren type #2
    SYSTEM_MODE_FUTURE_SIREN_3,   // 'K' — Future siren type #3
    SYSTEM_MODE_REPORT,           // 'D' — Report asked
    SYSTEM_MODE_VOICE,            // 'N' — P.A. (Public Address)
    SYSTEM_MODE_RESET,            // 'E' — Reset System
    SYSTEM_MODE_STANDBY,          // 'A' — Standby (Note: 'A' already used? See note below)
    SYSTEM_MODE_IDLE,             // ' ' or '0' — no active mode
    SYSTEM_MODE_UNKNOWN           // for error handling
} system_mode_t;

typedef struct {
    system_mode_t mode;                 // Current mode 
    bool amplifier_driver[10];          // Driver 1–10: true = OK
    bool not_used;                      // Reserved
    bool max_volume;                    // Max volume reached
    bool main_tone;                     // Main tone active
    bool secondary_tone;                // Secondary tone active
    bool operating_current;             // Output current OK
    bool battery_voltage;               // Battery voltage correct
    bool charger_unit;                  // Charger OK
    bool ac_voltage;                    // Mains present
    bool flood_sensor;                  // Flood detected
    bool door_sensor;                   // Door open
} system_status_t;

void system_status_init(void);
void system_status_reset(void);
char system_status_get_mode_char(void);
void system_status_set_mode(system_mode_t mode);

//int system_get_volume(void);
//void system_set_volume(int level);
//int volume_bars_to_db(uint8_t bars);
//uint8_t volume_db_to_bars(int db);

extern system_status_t system_status;

#endif // SYSTEM_STATUS_H
