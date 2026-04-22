#ifndef INC_LCD_LCD_STRINGS_MENU_H_
#define INC_LCD_LCD_STRINGS_MENU_H_

#include <stdint.h>
#include "lcd_lang.h"

#define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))

typedef const char* (*GetStringHandler)(uint8_t task, Language lang);

typedef enum {
	STR_HEADER_MENU = 0,
	STR_HEADER_FAULT_IND,
	STR_HEADER_SIREN,
	STR_HEADER_ALARM_INFO,
	STR_HEADER_MESSAGES,
	STR_HEADER_PLAY_MESSAGES,
	STR_HEADER_ANNOUNCEMENT,
	STR_HEADER_TEST,
	STR_HEADER_BATTERIES_TEST,
	STR_HEADER_APLIFIERS_TEST,
	STR_HEADER_DRIVERS_TEST,
	STR_HEADER_GENERATE_SINUS,
	STR_HEADER_SINUS_INFO,
	STR_HEADER_REPORT,
	STR_HEADER_MAINTENANCE,
	STR_HEADER_TIME_AND_DATE,
	STR_HEADER_LANGUAGES,
	STR_HEADER_MOTOROLA
} MenuHeaderStrings;

typedef enum {
	STR_ROOT_ITEM_SIREN = 0,
	STR_ROOT_ITEM_MESSAGES,
	STR_ROOT_ITEM_ANNOUNCEMENT,
	STR_ROOT_ITEM_TESTS,
	STR_ROOT_ITEM_REPORT,
	STR_ROOT_ITEM_MAINTENANCE,
	STR_TEST_ITEM_SILENT_TEST = 0,
	STR_TEST_ITEM_BATTERIES_TEST,
	STR_TEST_ITEM_APLIFIERS_TEST,
	STR_TEST_ITEM_DRIVERS_TEST,
	STR_TEST_ITEM_SINUS_TEST,
	STR_TIME_AND_DATE = 0,
	STR_LANGUAGES,
	STR_SINUS_420HZ_120S = 0,
	STR_SINUS_1000HZ_120S,
	STR_SINUS_1020HZ_120S,
	STR_SINUS_20000HZ_120S,
	STR_SINUS_836HZ_856HZ_60S,
	STR_SINUS_ALARM_90S,
	STR_SINUS_ALL_CLEAR_90S,
	STR_SINUS_ALL_CLEAR_120S,
	STR_SINUS_ABC_120S,
	STR_SERVICE_EMPTY_DIR_STR = 0
} MenuItemStrings;

const char* get_menu_header_str(uint8_t task, Language lang);
const char* get_root_menu_items_str(uint8_t task, Language lang);
const char* get_test_menu_items_str(uint8_t task, Language lang);
const char* get_maintenance_menu_items_str(uint8_t task, Language lang);
const char* get_sinus_menu_items_str(uint8_t task, Language lang);
const char* get_service_str(uint8_t task, Language lang);
//const char* get_menu_items_str(uint8_t task, Language lang);

extern const uint8_t MENU_HEADER_ITEM_COUNT;
extern const uint8_t ROOT_MENU_ITEM_COUNT;
extern const uint8_t TEST_MENU_ITEM_COUNT;
extern const uint8_t MAINTENCE_MENU_ITEM_COUNT;
extern const uint8_t SINUS_ITEM_COUNT;
extern const uint8_t SERVICE_COUNT;
//extern const uint8_t MENU_ITEMS_COUNT;

#endif /* INC_LCD_LCD_STRINGS_MENU_H_ */
