#include "lcd_strings_menu.h"

static const char* menu_header_str[][LANG_COUNT] = {
  { "### Menu ###", "### š”˜‰ˆ ###" },
  { "### Fault indication ###", "### ‡‰……‰ š—Œ„ ###" },
  { "### Siren ###", "### ‘‰˜„ ###" },
  { "### Alarm info ###", "### Ž‰ƒ’ €†’—„ ###" },
  { "### Messages ###", "### „…ƒ’…š ###" },
  { "### Play message ###", "### „™Ž’š „…ƒ’„ ###" },
  { "### Announcement ###", "### ‹˜‰†„ ###" },
  { "### Tests ###", "### ƒ‰—…š ###" },
  { "### Batteries Test ###", "### ƒ‰—š ‘…ŒŒ…š ###" },
  { "### Amplifires Test ###", "### ƒ‰—š Ž‚˜‰ ###" },
  { "### Drivers Test ###", "### ƒ‰—š ƒ˜‰‰˜‰ ###" },
  { "### Generate sinus ###", "### ‰–‰˜š ‘‰…‘ ###" },
  { "### Sinus info ###", "### Ž‰ƒ’ ‘‰…‘ ###" },
  { "### Report ###", "### ƒ…‡ ###" },
  { "### Maintenance ###", "### š‡†…—„ ###" },
  { "### Time and Date ###", "### ™’„ …š€˜‰Š ###" },
  { "### Language ###", "### ™”„ ###" },
  { "### SW update ###", "### ’ƒ‹… „š…‹„ ###" },
  { "### Volume ###", "### ”‡ ###" },
  { "### Motorola ###", "### Ž…ˆ…˜…Œ„ ###" },
  { "### Confirming ###", "### Ž€™˜ ###" }
};
const uint8_t MENU_HEADER_ITEM_COUNT = ARRAY_SIZE(menu_header_str);

static const char* root_menu_items_str[][LANG_COUNT] = {
  { "Siren", "‘‰˜„" },
  { "Messages", "„…ƒ’…š" },
  { "Announcement", "‹˜‰†„" },
  { "Test", "ƒ‰—„" },
  { "Report", "ƒ…‡" },
  { "Maintenance", "š‡†…—„" }
};
const uint8_t ROOT_MENU_ITEM_COUNT = ARRAY_SIZE(root_menu_items_str);

static const char* test_menu_items_str[][LANG_COUNT] = {
  { "Silent Test", "ƒ‰—„ ™—ˆ„" },
  { "Batteries Test", "ƒ‰—š ‘…ŒŒ…š" },
  { "Amplifiers Test", "ƒ‰—š Ž‚˜‰" },
  { "Drivers Test", "ƒ‰—š ƒ˜‰‰˜‰" },
  { "Sinus Test", "ƒ‰—š ‘‰…‘" }
};
const uint8_t TEST_MENU_ITEM_COUNT = ARRAY_SIZE(test_menu_items_str);

static const char* maintenance_menu_items_str[][LANG_COUNT] = {
  { "Time and Date", "™’„ …š€˜‰Š" },
  { "Language select", "‡‰˜š ™”„" },
  { "Software update", "’ƒ‹… „š…‹„" },
  { "Volume update", "’ƒ‹… „š…‹„" }
};
const uint8_t MAINTENCE_MENU_ITEM_COUNT = ARRAY_SIZE(maintenance_menu_items_str);

static const char* sinus_menu_items_str[][LANG_COUNT] = {
  { "420Hz 120s", "420Hz 120s" },
  { "1000Hz 120s", "1000Hz 120s" },
  { "1020Hz 120s", "1020Hz 120s" },
  { "20000Hz 120s", "20000Hz 120s" },
  { "836Hz-856Hz 60s", "836Hz-856Hz 60s" },
  { "ALARM 90s", "€†’—„ 90s" },
  { "ALL_CLEAR 60s", "‘‰…_€†’—„ 60s" },
  { "ALL_CLEAR 120s", "‘‰…_€†’—„ 120s" },
  { "ABC 120s", "ABC 120s" }
};
const uint8_t SINUS_ITEM_COUNT = ARRAY_SIZE(sinus_menu_items_str);

static const char* service_str[][LANG_COUNT] = {
  { "    Serial", "Ž‘‰ ‘‰ƒ…˜‰" },
  { "The directory is empty", "„‘”˜‰‰„ ˜‰—„" }
};
const uint8_t SERVICE_COUNT = ARRAY_SIZE(service_str);

static const char* sel_confirm_str[][LANG_COUNT] = {
  { "YES", "‹" },
  { "NO", "Œ€" }
};
const uint8_t SEL_CONFIRM_COUNT = ARRAY_SIZE(sel_confirm_str);

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

const char* get_sel_confirm_str(uint8_t task, Language lang)
{
  if (task >= SEL_CONFIRM_COUNT) return "";
  return sel_confirm_str[task][lang];
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


