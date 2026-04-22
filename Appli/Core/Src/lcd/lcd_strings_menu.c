#include "lcd_strings_menu.h"

static const char* menu_header_str[][LANG_COUNT] = {
	{ "### Menu ###", "### -Menu- ###" },
	{ "### Fault indication ###", "### -Fault indication- ###" },
	{ "### Siren ###", "### -Siren- ###" },
	{ "### Alarm info ###", "### -Alarm info- ###" },
	{ "### Messages ###", "### -Messages- ###" },
	{ "### Play message ###", "### -Play message- ###" },
	{ "### Announcement ###", "### -Announcement- ###" },
	{ "### Tests ###", "### -Tests- ###" },
	{ "### Batteries Test ###", "### -Batteries Test- ###" },
	{ "### Amplifires Test ###", "### -Amplifires Test- ###" },
	{ "### Drivers Test ###", "### -Drivers Test- ###" },
	{ "### Generate sinus ###", "### -Generate sinus- ###" },
	{ "### Sinus info ###", "### -Sinus info- ###" },
	{ "### Report ###", "### -Report- ###" },
	{ "### Maintenance ###", "### -Maintenance- ###" },
	{ "### Time and Date ###", "### -Time and Date- ###" },
	{ "### Language ###", "### -Language- ###" },
	{ "### Motorola ###", "### -Motorola- ###" }
};
const uint8_t MENU_HEADER_ITEM_COUNT = ARRAY_SIZE(menu_header_str);

static const char* root_menu_items_str[][LANG_COUNT] = {
	{ "Siren", "-Siren-" },
	{ "Messages", "-Messages-" },
	{ "Announcement", "-Announcement-" },
	{ "Test", "-Test-" },
	{ "Report", "-Report-" },
	{ "Maintenance", "-Maintenance-" }
};
const uint8_t ROOT_MENU_ITEM_COUNT = ARRAY_SIZE(root_menu_items_str);

static const char* test_menu_items_str[][LANG_COUNT] = {
	{ "Silent Test", "-Silent Test-" },
	{ "Batteries Test", "-Batteries Test-" },
	{ "Amplifiers Test", "-Amplifiers Test-" },
	{ "Drivers Test", "-Drivers Test-" },
	{ "Sinus Test", "-Sinus Test-" }
};
const uint8_t TEST_MENU_ITEM_COUNT = ARRAY_SIZE(test_menu_items_str);

static const char* maintenance_menu_items_str[][LANG_COUNT] = {
	{ "Time and Date", "Time and Date" },
	{ "Language select", "Language select" }
};
const uint8_t MAINTENCE_MENU_ITEM_COUNT = ARRAY_SIZE(maintenance_menu_items_str);

static const char* sinus_menu_items_str[][LANG_COUNT] = {
	{ "420Hz 120s", "-420Hz 120s-" },
	{ "1000Hz 120s", "-1000Hz 120s-" },
	{ "1020Hz 120s", "-1020Hz 120s-" },
	{ "20000Hz 120s", "-20000Hz 120s-" },
	{ "836Hz and 856Hz 60s", "-836Hz and 856Hz 60s-" },
	{ "ALARM 90s", "-ALARM 90s-" },
	{ "ALL_CLEAR 60s", "-ALL_CLEAR 60s-" },
	{ "ALL_CLEAR 120s", "-ALL_CLEAR 120s-" },
	{ "ABC 120s", "-ABC 120s-" }
};
const uint8_t SINUS_ITEM_COUNT = ARRAY_SIZE(sinus_menu_items_str);

static const char* service_str[][LANG_COUNT] = {
	{ "The directory is empty", "-The directory is empty-" }
};
const uint8_t SERVICE_COUNT = ARRAY_SIZE(service_str);

const char* get_menu_header_str(uint8_t task, Language lang)
{
    if (task >= MENU_HEADER_ITEM_COUNT) return "";
    return menu_header_str[task][lang];
}

const char* get_root_menu_items_str(uint8_t task, Language lang)
{
    if (task >= ROOT_MENU_ITEM_COUNT) return "";
    return root_menu_items_str[task][lang];
}

const char* get_test_menu_items_str(uint8_t task, Language lang)
{
    if (task >= TEST_MENU_ITEM_COUNT) return "";
    return test_menu_items_str[task][lang];
}

const char* get_maintenance_menu_items_str(uint8_t task, Language lang)
{
    if (task >= MAINTENCE_MENU_ITEM_COUNT) return "";
    return maintenance_menu_items_str[task][lang];
}

const char* get_sinus_menu_items_str(uint8_t task, Language lang)
{
    if (task >= SINUS_ITEM_COUNT) return "";
    return sinus_menu_items_str[task][lang];
}

const char* get_service_str(uint8_t task, Language lang)
{
    if (task >= SERVICE_COUNT) return "";
    return service_str[task][lang];
}

//char* get_menu_items_str(uint8_t task, Language lang)
//{
//    if (task >= MENU_ITEMS_COUNT) return "";
//    const char* text = menu_items_str[task][lang];
//
//    static char item_str[64];
//
//    if(GetLanguage() == LANG_EN)
//	{
//		sprintf(item_str[task], "%d. %s", task+1, text);
//	}
//	else if(GetLanguage() == LANG_EN)
//	{
//		sprintf(item_str[task], "%s .%d", text, task+1);
//	}
//    return item_str;
//}


