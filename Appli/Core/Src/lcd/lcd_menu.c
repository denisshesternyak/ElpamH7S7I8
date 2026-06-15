#include "lcd_menu.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"

#include "ff.h"
#include <string.h>
#include <stdio.h>
#include "lcd_color_rgb565.h"
#include "Speaker-1_80x74.h"
#include "m2_80x74.h"
#include "lcd_widget_progress_bar.h"
#include "lcd_widget_volume_indicator.h"
#include "lcd_widget_volume.h"
#include "lcd_widget_date_time.h"
#include "lcd_widget_faults_indicator.h"
#include "lcd_widget_batteries_indicator.h"
#include "lcd_widget_test_drivers_indicator.h"
#include "lcd_widget_test_ampl_indicator.h"
#include "lcd_widget_report_indicator.h"
#include "lcd_widget_motorola.h"
#include "lcd_widget_volume.h"
#include "lcd_widget_logging.h"
#include "app_freertos.h"
#include "rtc.h"
#include "sdfs.h"
#include "analog.h"
#include "system_status.h"
#include "metadata.h"
#include "lcd_widget_password.h"
#include "tester.h"

//bool isResetPasswordAfterIdle = false;

static bool isBacklightOn = false;

#define MAX_MENU_POOL 40

static Menu menuPool[MAX_MENU_POOL];
static uint8_t menuPoolIndex = 0;

static char listFilenames[MAX_MENU_ITEMS][FF_MAX_LFN] __attribute__((section(".extram")));
//const char *selectedFile = NULL;

Menu *currentMenu = NULL;
Menu *idleMenu = NULL;
Menu *rootMenu = NULL;
Menu *messagesMenu = NULL;
Menu *sirenMenu = NULL;
Menu *announcementMenu = NULL;
Menu *testMenu = NULL;
Menu *reportMenu = NULL;
Menu *messagePlayMenu = NULL;
Menu *batteriesTestMenu = NULL;
Menu *apmplifiresTestMunu = NULL;
Menu *driversTestMenu = NULL;
Menu *alarm_info_menu = NULL;
Menu *maintenanceMenu = NULL;
Menu *clockMenu = NULL;
Menu *languageMenu = NULL;
Menu *softwareMenu = NULL;
Menu *sinusMenu = NULL;
Menu *sinusInfoMenu = NULL;
Menu *motorolaInfoMenu = NULL;
Menu* passwordMenu = NULL;
Menu* volumeMenu = NULL;
Menu* confirmMenu = NULL;
Menu* loggingMenu = NULL;
//static uint8_t volumeValue = 10;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
MenuImage menu_speaker_img = {
    .image = speaker_img,
    .x = ((HX8357_TFTWIDTH / 2) - (SPEAKER_IMG_WIDTH / 2)),
    .y = 160,
    .w = SPEAKER_IMG_WIDTH,
    .h = SPEAKER_IMG_HEIGHT };
//////////////////////////////////////////////////////////////////////////////////////////////////////////

MenuImage menu_microfon_img = {
    .image = microfon_img,
    .x = ((HX8357_TFTWIDTH / 2) - (MICROFON_IMG_WIDTH / 2)),
    .y = 120,
    .w = MICROFON_IMG_WIDTH,
    .h = MICROFON_IMG_HEIGHT };
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_VISIBLE_ITEMS 8

#define TIME_HEIGHT 9

#define MENU_ITEM_HEIGHT 26

#define DEBUG_INFO_Y 290
#define STATUS_BAR_HEIGHT 24  
#define LOGO_X_POS 10
#define LOGO_Y_POS 12

#define SERIAL_X_POS 340     
#define SERIAL_Y_POS 28

#define STATUS_BAR_LINE_Y_POS  51
#define TITLE_MENU_Y_POS  (STATUS_BAR_LINE_Y_POS + 15)

#define TEXT_Y_POS  (TITLE_MENU_Y_POS + 45)
#define IDLE_Y_POS  (TITLE_MENU_Y_POS + 35)
#define BAT_Y_POS  (TITLE_MENU_Y_POS + 35)
#define AMP_Y_POS  (TITLE_MENU_Y_POS + 50)
#define DRV_Y_POS  (TITLE_MENU_Y_POS + 50)
#define REPORT_Y_POS  (TITLE_MENU_Y_POS + 50)

#define MENU_BASE_X 20
#define MENU_BASE_Y (TITLE_MENU_Y_POS + 35)

#define DIR_IS_EMPTY_Y_POS  147

#define SERIAL_NUMBER "123456"

///////////////////////////////////////////////////////////////////
void RunSilentTest (void);
void RunBatteriesTest (void);
void RunAmplifiresTest (void);
void RunDriversTest (void);
static void lcd_audio_notify(AudioEvent_t event, AudioType_t type);
static void lcd_notify_start_task_low (AudioType_t type,
				  SinTask_t task,
				  const char *name);
static void lcd_notify_arming (bool val);
static void menu_init_language (void);

static bool hot_key_handle_button (KeyEvent_t event);
static void passwordMenu_handle_button_press(KeyEvent_t event);
static void handle_button_press (KeyEvent_t event);
static void clockMenu_handle_button_press (KeyEvent_t event);
static void languageMenu_handle_button_press (KeyEvent_t event);
//static void sinus_info_menu_handler(KeyEvent_t event);
static void alarm_info_menu_handler (KeyEvent_t event);
static void message_info_menu_handler (KeyEvent_t event);
static void sinus_info_menu_handler (KeyEvent_t event);
static void idle_menu_handler (KeyEvent_t event);
static void volume_control_handler (KeyEvent_t event);
static void maintenance_handle_button_press (KeyEvent_t event);
static void volumeMenu_handle_button_press (KeyEvent_t event);
static void confirmMenu_handle_button_press (KeyEvent_t event);
static void loggingMenu_handle_button_press (KeyEvent_t event);
///////////////////////////////////////////////////////////////////

static void clear_position (Menu *menu);
static void clear_menu (Menu *menu);
static void basic_init_menu (Menu *menu);

static void button_up_handler (void);
static void button_down_handler (void);
static void button_enter_handler (void);
static void button_esc_handler (void);

static void draw_status_bar (void);
static void draw_menuScreen (bool forceFullRedraw);
static void display_menu_item (uint8_t visualIndex,
			       uint8_t index,
			       const MenuItem *item,
			       bool selected,
			       bool dummy);
static void menu_draw_image (Menu *m);
static void MenuLoadSDCardMessages (void);
static void MenuLoadSDCardSirens (void);
static void MenuLoadSDCardSoftWare (void);
static void prepare_announcement (void);
static void prepare_sinuse_items (void);
static void draw_menu_motorola (void);
static void lcd_inactivity_timeout ();

static void siren_info_prepare_action (void);
static void sinus_info_prepare_action (void);
static void draw_textFilename (void);
static void check_playing_and_stop ();

static void software_prepareAction (void);
typedef void (*confirm_handler_t) (void);
confirm_handler_t confirm_handler = NULL;

static state_handler_t is_playing_handler = NULL;
static state_handler_t is_stoped_handler = NULL;
static state_handler_t is_recording_handler = NULL;
static state_handler_t is_announcement_handler = NULL;
static state_handler_t is_arming_handler = NULL;
static state_handler_t is_motorola_handler = NULL;

static volume_indicator lcd_volume_indicator_handler = NULL;

static void BLK_ON ()
{
  HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_SET);
}
static void BLK_OFF ()
{
  HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_RESET);
}

static void clear_position (Menu *menu)
{
  menu->scrollOffset = 0;
  menu->currentSelection = 0;
  menu->oldSelection = 0;
  menu->oldOffset = 0;
}

static void clear_menu (Menu *menu)
{
  if (!menu)
    return;
  if (menu->type != MENU_TYPE_LIST)
    return;

  clear_position(menu);
  menu->itemCount = 0;

  for (int i = 0; i < MAX_MENU_ITEMS; ++i)
  {
    for (int lang = 0; lang < LANG_COUNT; ++lang)
    {
      menu->items[i].name[lang] = NULL;
    }

    menu->items[i].prepareAction = NULL;
    menu->items[i].postAction = NULL;
    menu->items[i].submenu = NULL;
  }
}

static void basic_init_menu (Menu *menu)
{
  if (!menu)
    return;

  menu->parent = NULL;
  menu->type = MENU_TYPE_UNK;
  menu->scrollOffset = 0;
  menu->itemCount = 0;
  menu->currentSelection = 0;
  menu->oldSelection = 0;
  menu->oldOffset = 0;

  for (int i = 0; i < MAX_MENU_ITEMS; ++i)
  {
    for (int lang = 0; lang < LANG_COUNT; ++lang)
    {
      menu->items[i].name[lang] = NULL;
    }
    menu->items[i].prepareAction = NULL;
    menu->items[i].postAction = NULL;
    menu->items[i].submenu = NULL;
  }

  for (int lang = 0; lang < LANG_COUNT; ++lang)
  {
    menu->screenText[lang] = NULL;
  }

  menu->textFilename = NULL;
  menu->imageData = NULL;
  menu->buttonHandler = NULL;
}

void InitMenuPool (void)
{
  for (int i = 0; i < MAX_MENU_POOL; ++i)
  {
    basic_init_menu(&menuPool[i]);
  }
  menuPoolIndex = 0;
}

void menu_init (void)
{
  InitMenuPool();

  idleMenu = &menuPool[menuPoolIndex++];
  rootMenu = &menuPool[menuPoolIndex++];
  sirenMenu = &menuPool[menuPoolIndex++];
  alarm_info_menu = &menuPool[menuPoolIndex++];
  messagesMenu = &menuPool[menuPoolIndex++];
  messagePlayMenu = &menuPool[menuPoolIndex++];
  announcementMenu = &menuPool[menuPoolIndex++];
  testMenu = &menuPool[menuPoolIndex++];
  batteriesTestMenu = &menuPool[menuPoolIndex++];
  apmplifiresTestMunu = &menuPool[menuPoolIndex++];
  driversTestMenu = &menuPool[menuPoolIndex++];
  reportMenu = &menuPool[menuPoolIndex++];
  maintenanceMenu = &menuPool[menuPoolIndex++];
  clockMenu = &menuPool[menuPoolIndex++];
  languageMenu = &menuPool[menuPoolIndex++];
  softwareMenu = &menuPool[menuPoolIndex++];
  sinusMenu = &menuPool[menuPoolIndex++];
  sinusInfoMenu = &menuPool[menuPoolIndex++];
  motorolaInfoMenu = &menuPool[menuPoolIndex++];
  passwordMenu = &menuPool[menuPoolIndex++];
  volumeMenu = &menuPool[menuPoolIndex++];
  confirmMenu = &menuPool[menuPoolIndex++];
  loggingMenu = &menuPool[menuPoolIndex++];

  idleMenu->parent = idleMenu;
  idleMenu->type = MENU_TYPE_IDLE;
  idleMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_FAULT_IND, LANG_EN);
  idleMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_FAULT_IND, LANG_HE);
  idleMenu->buttonHandler = idle_menu_handler;

//	///////////////////////////////////////////////
  rootMenu->parent = idleMenu;
  rootMenu->type = MENU_TYPE_LIST;
  rootMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_MENU, LANG_EN);
  rootMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_MENU, LANG_HE);
  rootMenu->buttonHandler = handle_button_press;
  rootMenu->items[0] = (MenuItem ) {
	  .name = {
	      get_root_menu_items_str(STR_ROOT_ITEM_SIREN, LANG_EN),
	      get_root_menu_items_str(STR_ROOT_ITEM_SIREN, LANG_HE) },
	  .prepareAction = &MenuLoadSDCardSirens,
	  .submenu = sirenMenu };
  rootMenu->items[1] = (MenuItem ) {
	  .name = {
	      get_root_menu_items_str(STR_ROOT_ITEM_MESSAGES, LANG_EN),
	      get_root_menu_items_str(STR_ROOT_ITEM_MESSAGES, LANG_HE) },
	  .prepareAction = &MenuLoadSDCardMessages,
	  .submenu = messagesMenu };
  rootMenu->items[2] = (MenuItem ) {
	  .name = {
	      get_root_menu_items_str(STR_ROOT_ITEM_ANNOUNCEMENT, LANG_EN),
	      get_root_menu_items_str(STR_ROOT_ITEM_ANNOUNCEMENT, LANG_HE) },
	  .prepareAction = &prepare_announcement,
	  .submenu = announcementMenu };
  rootMenu->items[3] = (MenuItem ) {
	  .name = {
	      get_root_menu_items_str(STR_ROOT_ITEM_TESTS, LANG_EN),
	      get_root_menu_items_str(STR_ROOT_ITEM_TESTS, LANG_HE) },
	  .submenu = testMenu };
  rootMenu->items[4] = (MenuItem ) {
	  .name = {
	      get_root_menu_items_str(STR_ROOT_ITEM_REPORT, LANG_EN),
	      get_root_menu_items_str(STR_ROOT_ITEM_REPORT, LANG_HE) },
	  .submenu = reportMenu };
  rootMenu->items[5] = (MenuItem ) {
	  .name = {
	      get_root_menu_items_str(STR_ROOT_ITEM_MAINTENANCE, LANG_EN),
	      get_root_menu_items_str(STR_ROOT_ITEM_MAINTENANCE, LANG_HE) },
	  .submenu = passwordMenu };
//	  .submenu = maintenanceMenu };
  rootMenu->itemCount = ROOT_MENU_ITEM_COUNT;

//    /////////////////////////////////////////
  // "Siren"
  sirenMenu->parent = rootMenu;
  sirenMenu->type = MENU_TYPE_LIST;
  sirenMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_SIREN, LANG_EN);
  sirenMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_SIREN, LANG_HE);
  sirenMenu->buttonHandler = handle_button_press;

  alarm_info_menu->parent = sirenMenu;
  alarm_info_menu->type = MENU_TYPE_SIREN_INFO;
  alarm_info_menu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_ALARM_INFO, LANG_EN);
  alarm_info_menu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_ALARM_INFO, LANG_HE);
  alarm_info_menu->buttonHandler = alarm_info_menu_handler;

//    //-----------------------------------------------------------------------------------------------------------
  messagesMenu->parent = rootMenu;
  messagesMenu->type = MENU_TYPE_LIST;
  messagesMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_MESSAGES, LANG_EN);
  messagesMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_MESSAGES, LANG_HE);
  messagesMenu->buttonHandler = handle_button_press;

  // play message
  messagePlayMenu->parent = messagesMenu;
  messagePlayMenu->type = MENU_TYPE_MESSAGE_PLAY;
  messagePlayMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_PLAY_MESSAGES, LANG_EN);
  messagePlayMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_PLAY_MESSAGES, LANG_HE);
  messagePlayMenu->imageData = &menu_speaker_img;
  messagePlayMenu->buttonHandler = message_info_menu_handler;

//    //-----------------------------------------------------------------------------------------------------------
  announcementMenu->parent = rootMenu;
  announcementMenu->currentSelection = 0;
  announcementMenu->itemCount = 0;
  announcementMenu->scrollOffset = 0;
  announcementMenu->type = MENU_TYPE_ANNOUNCEMENT;
  announcementMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_ANNOUNCEMENT, LANG_EN);
  announcementMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_ANNOUNCEMENT, LANG_HE);
  announcementMenu->imageData = &menu_microfon_img;
  announcementMenu->buttonHandler = volume_control_handler;

  testMenu->parent = rootMenu;
  testMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_TEST, LANG_EN);
  testMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_TEST, LANG_HE);
  testMenu->type = MENU_TYPE_LIST;
  testMenu->buttonHandler = handle_button_press;
  testMenu->items[0] = (MenuItem ) {
	  .name = {
	      get_test_menu_items_str(STR_TEST_ITEM_SILENT_TEST, LANG_EN),
	      get_test_menu_items_str(STR_TEST_ITEM_SILENT_TEST, LANG_HE) },
	  .postAction = &RunSilentTest };
  testMenu->items[1] = (MenuItem ) {
	  .name = {
	      get_test_menu_items_str(STR_TEST_ITEM_BATTERIES_TEST, LANG_EN),
	      get_test_menu_items_str(STR_TEST_ITEM_BATTERIES_TEST, LANG_HE) },
	  .postAction = &RunBatteriesTest,
	  .submenu = batteriesTestMenu };
  testMenu->items[2] = (MenuItem ) {
	  .name = {
	      get_test_menu_items_str(STR_TEST_ITEM_APLIFIERS_TEST, LANG_EN),
	      get_test_menu_items_str(STR_TEST_ITEM_APLIFIERS_TEST, LANG_HE) },
	  .postAction = &RunAmplifiresTest,
	  .submenu = apmplifiresTestMunu };
  testMenu->items[3] = (MenuItem ) {
	  .name = {
	      get_test_menu_items_str(STR_TEST_ITEM_DRIVERS_TEST, LANG_EN),
	      get_test_menu_items_str(STR_TEST_ITEM_DRIVERS_TEST, LANG_HE) },
	  .postAction = &RunDriversTest,
	  .submenu = driversTestMenu };
  testMenu->items[4] = (MenuItem ) {
	  .name = {
	      get_test_menu_items_str(STR_TEST_ITEM_SINUS_TEST, LANG_EN),
	      get_test_menu_items_str(STR_TEST_ITEM_SINUS_TEST, LANG_HE) },
	  .prepareAction = &prepare_sinuse_items,
	  .submenu = sinusMenu };
  testMenu->itemCount = TEST_MENU_ITEM_COUNT;

//	//---------------------------------------------------------------
  batteriesTestMenu->parent = testMenu;
  batteriesTestMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_BATTERIES_TEST, LANG_EN);
  batteriesTestMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_BATTERIES_TEST, LANG_HE);
  batteriesTestMenu->type = MENU_TYPE_TEST_BAT;
  batteriesTestMenu->buttonHandler = handle_button_press;

//
//	//--------------------------------------------------------
  apmplifiresTestMunu->parent = testMenu;
  apmplifiresTestMunu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_APLIFIERS_TEST, LANG_EN);
  apmplifiresTestMunu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_APLIFIERS_TEST, LANG_HE);
  apmplifiresTestMunu->type = MENU_TYPE_TEST_AMP;
  apmplifiresTestMunu->buttonHandler = handle_button_press;

//
//	//-----------------------------------
  driversTestMenu->parent = testMenu;
  driversTestMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_DRIVERS_TEST, LANG_EN);
  driversTestMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_DRIVERS_TEST, LANG_HE);
  driversTestMenu->type = MENU_TYPE_TEST_DRIV;
  driversTestMenu->buttonHandler = handle_button_press;

  sinusMenu->parent = testMenu;
  sinusMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_GENERATE_SINUS, LANG_EN);
  sinusMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_GENERATE_SINUS, LANG_HE);
  sinusMenu->type = MENU_TYPE_LIST;
  sinusMenu->buttonHandler = handle_button_press;

  sinusInfoMenu->parent = sinusMenu;
  sinusInfoMenu->type = MENU_TYPE_SIREN_INFO;
  sinusInfoMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_SINUS_INFO, LANG_EN);
  sinusInfoMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_SINUS_INFO, LANG_HE);
//	sinusInfoMenu->buttonHandler = sinus_info_menu_handler;
  sinusInfoMenu->buttonHandler = sinus_info_menu_handler;

//	////////////////////////////////////////////////////////
  reportMenu->parent = rootMenu;
  reportMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_REPORT, LANG_EN);
  reportMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_REPORT, LANG_HE);
  reportMenu->type = MENU_TYPE_REPORT;
  reportMenu->buttonHandler = handle_button_press;

  maintenanceMenu->parent = rootMenu;
  maintenanceMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_MAINTENANCE, LANG_EN);
  maintenanceMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_MAINTENANCE, LANG_HE);
  maintenanceMenu->type = MENU_TYPE_LIST;
  maintenanceMenu->buttonHandler = maintenance_handle_button_press;
  maintenanceMenu->items[0] = (MenuItem ) {
	  .name = {
	      get_maintenance_menu_items_str(STR_TIME_AND_DATE, LANG_EN),
	      get_maintenance_menu_items_str(STR_TIME_AND_DATE, LANG_HE) },
	  .prepareAction = &DateTime_Prepare,
	  .submenu = clockMenu};
  maintenanceMenu->items[1] = (MenuItem ) {
	  .name = {
	      get_maintenance_menu_items_str(STR_LANGUAGES, LANG_EN),
	      get_maintenance_menu_items_str(STR_LANGUAGES, LANG_HE) },
	  .prepareAction = &menu_init_language,
	  .submenu = languageMenu };
  maintenanceMenu->items[2] = (MenuItem ) {
	  .name = {
	      get_maintenance_menu_items_str(STR_SOFTWARE_UPDATE, LANG_EN),
	      get_maintenance_menu_items_str(STR_SOFTWARE_UPDATE, LANG_HE) },
	  .prepareAction = &MenuLoadSDCardSoftWare,
	  .submenu = softwareMenu };
  maintenanceMenu->items[3] = (MenuItem ) {
	  .name = {
	      get_maintenance_menu_items_str(STR_VOLUME_UPDATE, LANG_EN),
	      get_maintenance_menu_items_str(STR_VOLUME_UPDATE, LANG_HE) },
	  .submenu = volumeMenu };
  maintenanceMenu->items[4] = (MenuItem ) {
	  .name = {
	      get_maintenance_menu_items_str(STR_LOGGING, LANG_EN),
	      get_maintenance_menu_items_str(STR_LOGGING, LANG_HE) },
	  .prepareAction = &Logging_Init,
	  .submenu = loggingMenu };
  maintenanceMenu->itemCount = MAINTENCE_MENU_ITEM_COUNT;

  clockMenu->parent = maintenanceMenu;
  clockMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_TIME_AND_DATE, LANG_EN);
  clockMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_TIME_AND_DATE, LANG_HE);
  clockMenu->type = MENU_TYPE_CLOCK;
  clockMenu->buttonHandler = clockMenu_handle_button_press;

  languageMenu->parent = maintenanceMenu;
  languageMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_LANGUAGES, LANG_EN);
  languageMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_LANGUAGES, LANG_HE);
  languageMenu->type = MENU_TYPE_LIST;
  languageMenu->buttonHandler = languageMenu_handle_button_press;

  softwareMenu->parent = maintenanceMenu;
  softwareMenu->type = MENU_TYPE_LIST;
  softwareMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_SOFTWARE, LANG_EN);
  softwareMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_SOFTWARE, LANG_HE);
  softwareMenu->buttonHandler = handle_button_press;

  motorolaInfoMenu->parent = rootMenu;
  motorolaInfoMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_MOTOROLA, LANG_EN);
  motorolaInfoMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_MOTOROLA, LANG_HE);
  motorolaInfoMenu->type = MENU_TYPE_MOTOROLA;

  passwordMenu->parent = rootMenu;
  passwordMenu->type = MENU_TYPE_PASSWORD;
  passwordMenu->buttonHandler = passwordMenu_handle_button_press;

  volumeMenu->parent = maintenanceMenu;
  volumeMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_VOLUME, LANG_EN);
  volumeMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_VOLUME, LANG_HE);
  volumeMenu->type = MENU_TYPE_VOLUME;
  volumeMenu->buttonHandler = volumeMenu_handle_button_press;

  loggingMenu->parent = maintenanceMenu;
  loggingMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_LOG, LANG_EN);
  loggingMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_LOG, LANG_HE);
  loggingMenu->type = MENU_TYPE_LOG;
  loggingMenu->buttonHandler = loggingMenu_handle_button_press;

  confirmMenu->parent = rootMenu;
  confirmMenu->type = MENU_TYPE_LIST;
  confirmMenu->screenText[LANG_EN] = get_menu_header_str(STR_HEADER_CONFIRM, LANG_EN);
  confirmMenu->screenText[LANG_HE] = get_menu_header_str(STR_HEADER_CONFIRM, LANG_HE);
  confirmMenu->buttonHandler = confirmMenu_handle_button_press;
  confirmMenu->items[0] = (MenuItem ) {
  	  .name = {
  	      get_sel_confirm_str(STR_SEL_CONFIRM_YES, LANG_EN),
	      get_sel_confirm_str(STR_SEL_CONFIRM_YES, LANG_HE) } };
  confirmMenu->items[1] = (MenuItem ) {
  	  .name = {
  	      get_sel_confirm_str(STR_SEL_CONFIRM_NO, LANG_EN),
	      get_sel_confirm_str(STR_SEL_CONFIRM_NO, LANG_HE) } };
  confirmMenu->itemCount = SEL_CONFIRM_COUNT;

  BLK_ON();
  isBacklightOn = true;
  osTimerStart(BacklightTimerHandle, INACTIVITY_TIMEOUT_MS);

  currentMenu = rootMenu;

  tester_init();
  Volume_Init();
  draw_status_bar();
  update_date_time();
  draw_menuScreen(true);
}

static void menu_draw_image (Menu *menu)
{
  if (!menu->imageData)
    return;

  hx8357_draw_image(menu->imageData->x, menu->imageData->y, menu->imageData->w, menu->imageData->h, (const uint16_t*) menu->imageData->image);
}

//void MenuActionSystemReset(void)
//{
//
//    NVIC_SystemReset();
//}

void RunSilentTest (void)
{
//  LOG_DEBUG("12v %dmv, 24v %dmv, cur %d, amp %d, drv %d",
//		  getRData(1, get_adc_value(ADC_12V)),
//		  getRData(1, get_adc_value(ADC_24V)),
//		  getRData(1, get_adc_value(ADC_CURRENT_MEAS)),
//		  getRData(1, get_adc_value(ADC_AMPLIFIER_MEAS)),
//		  getRData(1, get_adc_value(ADC_DRIVER_MEAS)));

  currentMenu = batteriesTestMenu;
  clear_position(currentMenu);
  draw_menuScreen(true);
  RunBatteriesTest();

  osDelay(2000);

  currentMenu = apmplifiresTestMunu;
  clear_position(currentMenu);
  draw_menuScreen(true);
  RunAmplifiresTest();

  osDelay(2000);

  currentMenu = driversTestMenu;
  clear_position(currentMenu);
  draw_menuScreen(true);
  RunDriversTest();

  osDelay(2000);

  currentMenu = testMenu; //driversTestMenu->parent;
  clear_position(currentMenu);
  draw_menuScreen(true);
}

void RunBatteriesTest (void)
{
  bool is_12v = check_voltage(ADC_12V, ADC_12V_DIV, ADC_TASK_12V, 500);
  bool is_24v = check_voltage(ADC_24V, ADC_24V_DIV, ADC_TASK_24V, 1000);

  BatteriesDisplay_SetStatus(SYSTEM_MAINS, is_12v && is_24v);
  BatteriesDisplay_SetStatus(SYSTEM_UNDER_VO, IS_OVER_VO);
  BatteriesDisplay_SetStatus(SYSTEM_OVER_VO, IS_UNDER_VO);
}

void RunAmplifiresTest (void)
{
//  audio_notify_start_task_low(AUDIO_QUIET, SINUS_420HZ_120S, NULL);
  OSC_ON;

  for(uint8_t i = 0; i < AMP_COUNT; i++)
  {
    bool res = tester_check_amplifier(i);
    TestAmplDisplay_SetStatus(i, res);
  }

  OSC_OFF;
//  audio_notify_stop_task_low(AUDIO_QUIET, SINUS_420HZ_120S, NULL);
}

void RunDriversTest (void)
{
//  audio_notify_start_task_low(AUDIO_QUIET, SINUS_420HZ_120S, NULL);
  OSC_ON;

  for(uint8_t i = 0; i < DRV_COUNT; i++)
  {
    bool res = tester_check_driver(i);
    TestDrvDisplay_SetStatus(i, res);
  }

  OSC_OFF;
//  audio_notify_stop_task_low(AUDIO_QUIET, SINUS_420HZ_120S, NULL);
}

void MenuLoadSDCardSirens (void)
{
  if (!sirenMenu)
    return;

  clear_menu(sirenMenu);

  uint8_t count = 0;
  sdfs_list_alarms(listFilenames, &count);

  for (uint8_t i = 0; i < count && i < MAX_MENU_ITEMS; ++i)
  {
//    strncpy(listFilenames[i], list[i], FF_MAX_LFN);

    MenuItem *item = &sirenMenu->items[i];
    for (uint8_t j = 0; j < LANG_COUNT; j++)
    {
      item->name[j] = listFilenames[i];
    }
    item->submenu = alarm_info_menu;
    item->prepareAction = &siren_info_prepare_action;
  }

  sirenMenu->itemCount = count;
}

void MenuLoadSDCardSoftWare (void)
{
  if (!softwareMenu)
    return;

  clear_menu(softwareMenu);

  uint8_t count = 0;
  sdfs_list_firmware(listFilenames, &count);

  for (uint8_t i = 0; i < count && i < MAX_MENU_ITEMS; ++i)
  {
    MenuItem *item = &softwareMenu->items[i];
    for (uint8_t j = 0; j < LANG_COUNT; j++)
    {
      item->name[j] = listFilenames[i];
    }
    item->submenu = confirmMenu;
  }

  confirm_handler = software_prepareAction;
  confirmMenu->parent = softwareMenu;
  softwareMenu->itemCount = count;
}


void MenuLoadSDCardMessages (void)
{
  if (!messagesMenu || !messagePlayMenu)
    return;

  clear_menu(messagesMenu);

  uint8_t count = 0;
  sdfs_list_messages(listFilenames, &count);

  for (uint8_t i = 0; i < count && i < MAX_MENU_ITEMS; ++i)
  {
//    strncpy(listFilenames[i], dummyFilenames[i], FF_MAX_LFN);

    MenuItem *item = &messagesMenu->items[i];
    for (uint8_t j = 0; j < LANG_COUNT; j++)
    {
      item->name[j] = listFilenames[i];
    }
    item->submenu = messagePlayMenu;
    item->prepareAction = &siren_info_prepare_action;
  }

  messagesMenu->itemCount = count;
}

static void software_prepareAction (void)
{
  if (!softwareMenu)
    return;

  MenuItem *item = &softwareMenu->items[softwareMenu->currentSelection];
  if (!item)
    return;

  const char *fw_src = item->name[GetLanguage()];

  char fw_path[32];

  if(!sdfs_prepare_to_update(fw_src, UPDATE_FILE, fw_path, sizeof(fw_path)))
  {
    LOG_ERROR("Copy FW failed with error");
    return;
  }

  if(!metadata_path_update(fw_path, sizeof(fw_path)))
  {
    LOG_ERROR("Write metadata status failed");
    return;
  }

  LOG_INFO("FW is ready to update: %s", fw_src);
  LOG_INFO("System restart");
  osDelay(10);

  HAL_NVIC_SystemReset();
}

static void prepare_sinuse_items (void)
{
  if (!sinusMenu)
    return;

  clear_menu(sinusMenu);

  for (uint8_t i = 0; i < SINUS_ITEM_COUNT; i++)
  {
    MenuItem *item = &sinusMenu->items[i];
    for (uint8_t j = 0; j < LANG_COUNT; j++)
    {
      item->name[j] = get_sinus_menu_items_str(i, j);
    }

    item->submenu = sinusInfoMenu;
    item->prepareAction = &sinus_info_prepare_action;
  }

  sinusMenu->itemCount = SINUS_ITEM_COUNT;
}

void menu_init_language (void)
{
  if (!languageMenu)
    return;

  clear_menu(languageMenu);

  for (uint8_t i = 0; i < LANG_COUNT && i < MAX_MENU_ITEMS; ++i)
  {
    char langSel = (GetLanguage() == i) ? '*' : ' ';
    sprintf(listFilenames[i], "%s %c", LanguageToString(i), langSel);

    for (uint8_t j = 0; j < LANG_COUNT; j++)
    {
     languageMenu->items[i].name[j] = listFilenames[i];
    }
  }

  languageMenu->itemCount = LANG_COUNT;
}

static void siren_info_prepare_action (void)
{
  if (!currentMenu || currentMenu->currentSelection >= currentMenu->itemCount)
    return;

  MenuItem *item = &currentMenu->items[currentMenu->currentSelection];
  if (!item || !item->submenu)
    return;

  item->submenu->textFilename = item->name[GetLanguage()];

  lcd_notify_start_task_low(AUDIO_SD, SINUS_NONE, item->name[GetLanguage()]);

  MenuResetProgressBar();
}

static void sinus_info_prepare_action (void)
{
  if (!currentMenu || currentMenu->currentSelection >= currentMenu->itemCount)
    return;

  MenuItem *item = &currentMenu->items[currentMenu->currentSelection];
  if (!item || !item->submenu)
    return;

  item->submenu->textFilename = item->name[GetLanguage()];

  lcd_notify_start_task_low(AUDIO_SIN, (SinTask_t) currentMenu->currentSelection, item->name[GetLanguage()]);

  MenuResetProgressBar();
}

static void prepare_announcement (void)
{
  lcd_audio_notify(AUDIO_START, AUDIO_MIC);
}

void Draw_MENU_TYPE_IDLE ()
{
  FaultsDisplay_DrawAll(MENU_BASE_X, IDLE_Y_POS);

//  bool is_12v = check_voltage(ADC_12V, ADC_12V_DIV, ADC_TASK_12V, 500);
//  bool is_24v = check_voltage(ADC_24V, ADC_24V_DIV, ADC_TASK_24V, 1000);
//
//  BatteriesDisplay_SetStatus(SYSTEM_MAINS, is_12v && is_24v);
//  FaultsDisplay_SetStatus(SYSTEM_MAINS, is_12v && is_24v);
//
//
//  FaultsDisplay_SetStatus(SYSTEM_POW_DET, IS_POW_DET);
//  FaultsDisplay_SetStatus(SYSTEM_SOL_DET, IS_SOL_DET);
//
//  FaultsDisplay_SetStatus(SYSTEM_PPF_PD0_DOOR, IS_PPF_PD0_DOOR);
//  FaultsDisplay_SetStatus(SYSTEM_PPF_PD1_FLOOD, IS_PPF_PD1_FLOOD);
}

void Draw_MENU_TYPE_ANNOUNCEMENT (void)
{
  menu_draw_image(currentMenu);
  uint8_t level, value;
  lcd_volume_indicator_handler(&level, &value);
  volume_indicator_set_level_silent(level, value);
  volume_indicators_draw();
}

void Draw_MENU_TYPE_SIREN_INFO (void)
{
  draw_textFilename();

  menu_draw_image(currentMenu);

  MenuDrawProgress(0);

  osDelay(3);
}

void Draw_MENU_TYPE_REPORT (void)
{
  ReportIndicator_DrawAll(MENU_BASE_X, REPORT_Y_POS);
}

void Draw_MENU_TYPE_TEST_BAT (void)
{
  BatteriesDisplay_DrawAll(MENU_BASE_X, BAT_Y_POS);
}

void Draw_MENU_TYPE_TEST_DRIV (void)
{
  TestDrvDisplay_DrawAll(MENU_BASE_X, DRV_Y_POS);
}

void Draw_MENU_TYPE_TEST_AMP (void)
{
  TestAmplDisplay_DrawAll(MENU_BASE_X, AMP_Y_POS);
}

void Draw_MENU_TYPE_MESSAGE_PLAY (void)
{
  draw_textFilename();

  menu_draw_image(currentMenu);

  MenuDrawProgress(0);

  osDelay(3);
}


static void draw_menu_motorola (void)
{
  motorola_draw();

//	osDelay(3);
}

static void display_menu_item (uint8_t visualIndex,
			       uint8_t index,
			       const MenuItem *item,
			       bool selected,
			       bool dummy)
{
  uint16_t y_pos = MENU_BASE_Y + (visualIndex * MENU_ITEM_HEIGHT);
  uint16_t text_color = selected ? COLOR_WHITE : COLOR_GREEN;
  uint16_t bg_color = selected ? COLOR_BLUE : COLOR_BLACK;

  hx8357_fill_rect(MENU_BASE_X, y_pos, hx8357_get_width() - (MENU_BASE_X * 2), MENU_ITEM_HEIGHT, bg_color);

  char upd_text[FF_MAX_LFN];
  Alignment align;
  Language lang = GetLanguage();
  const char *text = (dummy) ? "..." : item->name[GetLanguage()];

  FontDef *font = &Font_16x26;
  size_t len = (hx8357_get_width() - (MENU_BASE_X * 2)) / font->width;

  if (currentMenu == languageMenu || lang == LANG_EN)
  {
    snprintf(upd_text, len, "%d. %s", index + 1, text);
    align = ALIGN_LEFT;
  }
  else if (lang == LANG_HE)
  {
    char full[FF_MAX_LFN];
    snprintf(full, sizeof(full), "%s .%d", text, index + 1);

    size_t full_len = strlen(full);
    size_t max_chars = len;

    if (full_len <= max_chars)
    {
      strcpy(upd_text, full);
    }
    else
    {
      const char *start = full + full_len - max_chars;
      memcpy(upd_text, start, max_chars);
      upd_text[max_chars] = '\0';
    }

    align = ALIGN_RIGHT;
  }

  hx8357_write_alignedX_string(MENU_BASE_X, y_pos, upd_text, font, text_color, bg_color, align);
}

static void lcd_inactivity_timeout ()
{
  if (currentMenu != idleMenu)
  {
    currentMenu = idleMenu;
    draw_menuScreen(true);
    osTimerStart(BacklightTimerHandle, BACKLIGHT_TIMEOUT_MS);
  }
  else if (currentMenu == idleMenu && isBacklightOn)
  {
    BLK_OFF();
    isBacklightOn = false;
  }
}

void Draw_MENU_TYPE_LIST ()
{
  if (currentMenu->itemCount == 0)
  {
    const char *text = get_service_str(STR_SERVICE_EMPTY_DIR_STR, GetLanguage());
    hx8357_write_alignedX_string(0, DIR_IS_EMPTY_Y_POS, text, &Font_16x26, COLOR_YELLOW, COLOR_BLACK, ALIGN_CENTER);
    return;
  }

  uint8_t old_selection = currentMenu->oldSelection;
  uint8_t old_offset = currentMenu->oldOffset;

  uint8_t selection = currentMenu->currentSelection;
  uint8_t item_count = currentMenu->itemCount;
  uint8_t offset = currentMenu->scrollOffset;

  uint8_t visible_count =
      (item_count > MAX_VISIBLE_ITEMS) ? MAX_VISIBLE_ITEMS : item_count;
  uint8_t end_index = offset + visible_count;

  bool show_scroll_up = offset > 0;
  bool show_scroll_down = (offset + MAX_VISIBLE_ITEMS) < item_count;

  bool selection_changed = old_selection != selection;
  bool offset_changed = old_offset != offset;

  /*
   if (!selection_changed && !offset_changed)
   {
   return;
   }
   */
  if (selection_changed && !offset_changed)
  {
    if (old_selection >= offset && old_selection < end_index)
    {
      uint8_t visual_index = old_selection - offset;
      display_menu_item(visual_index, old_selection, &currentMenu->items[old_selection], false, false);
    }

    if (selection >= offset && selection < end_index)
    {
      uint8_t visual_index = selection - offset;
      display_menu_item(visual_index, selection, &currentMenu->items[selection], true, false);
    }
  }
  else
  {
    for (uint8_t i = offset; i < end_index; ++i)
    {
      uint8_t visual_index = i - offset;
      bool dummy = (show_scroll_up && visual_index == 0) || (show_scroll_down && visual_index == visible_count - 1);
      display_menu_item(visual_index, i, &currentMenu->items[i], (i == selection), dummy);
      osDelay(3);
    }
  }

  currentMenu->oldSelection = selection;
  currentMenu->oldOffset = offset;
}

void draw_menuScreen (bool forceFullRedraw)
{
  static Menu *prevMenu = NULL;

  if (forceFullRedraw || prevMenu != currentMenu)
  {
    uint16_t bg_color = COLOR_BLACK;

    hx8357_fill_rect(MENU_BASE_X, TITLE_MENU_Y_POS, hx8357_get_width() - (MENU_BASE_X * 2), hx8357_get_height() - TITLE_MENU_Y_POS, bg_color);
    osDelay(1);

    const char *text = currentMenu->screenText[GetLanguage()];
    if (text)
    {
      hx8357_write_alignedX_string(0, TITLE_MENU_Y_POS, text, &Font_16x26, COLOR_WHITE, bg_color, ALIGN_CENTER);
    }
  }

  if (currentMenu->type == MENU_TYPE_IDLE)
  {
    Draw_MENU_TYPE_IDLE();
  }
  else if (currentMenu->type == MENU_TYPE_PASSWORD)
  {
    Draw_MENU_TYPE_PASSWORD();
  }
  else if (currentMenu->type == MENU_TYPE_ANNOUNCEMENT)
  {
    Draw_MENU_TYPE_ANNOUNCEMENT();
  }
  else if (currentMenu->type == MENU_TYPE_SIREN_INFO)
  {
    Draw_MENU_TYPE_SIREN_INFO();
  }
  else if (currentMenu->type == MENU_TYPE_REPORT)
  {
    Draw_MENU_TYPE_REPORT();
  }
  else if (currentMenu->type == MENU_TYPE_TEST_BAT)
  {
    Draw_MENU_TYPE_TEST_BAT();
  }
  else if (currentMenu->type == MENU_TYPE_TEST_DRIV)
  {
    Draw_MENU_TYPE_TEST_DRIV();
  }
  else if (currentMenu->type == MENU_TYPE_TEST_AMP)
  {
    Draw_MENU_TYPE_TEST_AMP();
  }
  else if (currentMenu->type == MENU_TYPE_MESSAGE_PLAY)
  {
    Draw_MENU_TYPE_MESSAGE_PLAY();
  }
  else if (currentMenu->type == MENU_TYPE_LIST)
  {
    Draw_MENU_TYPE_LIST();
  }
  else if (currentMenu->type == MENU_TYPE_CLOCK)
  {
    Draw_MENU_TYPE_CLOCK();
  }
  else if (currentMenu->type == MENU_TYPE_VOLUME)
  {
    Volume_UpdateValue();
  }
  else if (currentMenu->type == MENU_TYPE_LOG)
  {
    Logging_Draw();
  }
  else if (currentMenu->type == MENU_TYPE_MOTOROLA)
  {
    draw_menu_motorola();
  }

  prevMenu = currentMenu;

  osDelay(1);

#ifdef DEBUG_PRINT_BUTTON_STATE_2
   // DrawDebugInfo(&lastButtonEvent);
#endif
}

//static char debugInfo[50];
//
//void DrawDebugInfo(const KeyEvent_t* event)
//{
//return;
//	//LCD_FillRectangle(0, DEBUG_INFO_Y, LCD_GetWidth(), LCD_GetHeight(), debug_bg_color);
//
//
//	sprintf(debugInfo,
//			//sizeof(debugInfo),
//		"Btn:%s %s Level:%d Sel:%d/%d",
//		ButtonToString(event->button),
//		ButtonActionToString(event->action),
//		0,//currentLevel,
//		currentMenu->currentSelection + 1,
//		currentMenu->itemCount);
//
//	LCD_WriteString(10, DEBUG_INFO_Y, debugInfo, &Font_7x10, COLOR_WHITE, COLOR_DARKGRAY);
//}

void draw_serial()
{
  char serialStr[20];
  const char* serial_str = get_service_str(STR_SERVICE_SERIAL_STR, GetLanguage());
  snprintf(serialStr, sizeof(serialStr), "%s: %s", serial_str, SERIAL_NUMBER);
  hx8357_write_alignedX_string(0, SERIAL_Y_POS, serialStr, &Font_11x18, COLOR_YELLOW, COLOR_BLACK, ALIGN_RIGHT);
}

void draw_status_bar ()
{
//	hx8357_fill_rect(0, 0, hx8357_get_width(), STATUS_BAR_LINE_Y_POS, COLOR_BLACK);
  hx8357_write_alignedX_string(0, LOGO_Y_POS, "EES-3000", &Font_16x26, COLOR_WHITE, COLOR_BLACK, ALIGN_LEFT);

  draw_serial();

  hx8357_fill_rect(0, STATUS_BAR_LINE_Y_POS - 1, hx8357_get_width(), 1, COLOR_GRAY);
}

void update_date_time ()
{
  if (!isBacklightOn)
    return;

  if (is_motorola_handler())
    motorola_update();

  if (is_announcement_handler())
    volume_indicator_blink_bar();

  DateTime_HeaderClock();

  return;
}

void update_progress_bar (uint8_t value)
{
  if (!is_playing_handler() || (currentMenu != sinusInfoMenu && currentMenu != alarm_info_menu && currentMenu != messagePlayMenu))
    return;
  MenuDrawProgress(value);
}

void change_screen (MenuType type)
{
  switch (type)
  {
    case MENU_TYPE_MOTOROLA:
      currentMenu = motorolaInfoMenu;
      draw_menuScreen(true);
      break;
    case MENU_TYPE_PREVIOUS:
      if (!currentMenu->parent)
	return;
      currentMenu = currentMenu->parent;
      draw_menuScreen(true);
      break;
    case MENU_TYPE_IDLE:
      lcd_inactivity_timeout();
    default:
      break;
  }
}

//void ShowUartCommand(void)
//{
//	//LCD_FillRectangle(0, 0, LCD_GetWidth(), STATUS_BAR_LINE_Y_POS -1, COLOR_BLACK);
//	LCD_WriteString(LOGO_X_POS, LOGO_Y_POS, "EES-3000", &Font_7x10, COLOR_WHITE, COLOR_BLACK);
//
//}

void menu_handle_button (KeyEvent_t event)
{
  if (!currentMenu || (event.button == BTN_NONE))
    return;

  if (!isBacklightOn)
  {
    if (is_motorola_handler())
    {
      currentMenu = motorolaInfoMenu;
      draw_menuScreen(true);
    }

    BLK_ON();
    isBacklightOn = true;
    osTimerStart(BacklightTimerHandle, BACKLIGHT_TIMEOUT_MS);
    return;
  }

  osTimerStart(BacklightTimerHandle, INACTIVITY_TIMEOUT_MS);

  if (hot_key_handle_button(event))
    return;

  if (currentMenu->buttonHandler)
  {
    currentMenu->buttonHandler(event);
    return;
  }
}

bool hot_key_handle_button (KeyEvent_t event)
{
  if (is_motorola_handler())
    return false;

  switch (event.button)
  {
    case BTN_TEST:
      if (currentMenu == testMenu)
	return true;
      check_playing_and_stop();
      currentMenu = testMenu;
      clear_position(currentMenu);
      draw_menuScreen(true);
      return true;
    case BTN_ANNOUNCEMENT:
      if (currentMenu == announcementMenu)
	return true;
//      player.is_fade_stoped = true;
      check_playing_and_stop();
      prepare_announcement();
      currentMenu = announcementMenu;
      clear_position(currentMenu);
      draw_menuScreen(true);
      return true;
    case BTN_MESSAGE:
      if (currentMenu == messagesMenu)
	return true;
      check_playing_and_stop();
      MenuLoadSDCardMessages();
      currentMenu = messagesMenu;
      clear_position(currentMenu);
      draw_menuScreen(true);
      return true;
    case BTN_ALARM:
      if (currentMenu == sirenMenu)
	return true;
      check_playing_and_stop();
      MenuLoadSDCardSirens();
      currentMenu = sirenMenu;
      clear_position(currentMenu);
      draw_menuScreen(true);
      return true;
    case BTN_ARM:
      lcd_notify_arming(true);
      return true;
    case BTN_CXL:
      check_playing_and_stop();
      return true;
    default:
      return false;
  }
  return false;
}

void handle_button_press (KeyEvent_t event)
{
  if (!currentMenu)
    return;

  switch (event.button)
  {
    case BTN_UP:
      button_up_handler();
      break;
    case BTN_DOWN:
      button_down_handler();
      break;
    case BTN_ENTER:
      button_enter_handler();
      break;
    case BTN_ESC:
      button_esc_handler();
      break;
    default:
      return;
  }
}

static void button_up_handler (void)
{
  if (!currentMenu || currentMenu->itemCount == 0)
    return;

  if (currentMenu->currentSelection == 0)
  {
    currentMenu->currentSelection = currentMenu->itemCount - 1;
    if (currentMenu->itemCount > MAX_VISIBLE_ITEMS)
      currentMenu->scrollOffset = currentMenu->itemCount - MAX_VISIBLE_ITEMS;
  }
  else
  {
    currentMenu->currentSelection--;

    if (currentMenu->currentSelection < currentMenu->itemCount - MAX_VISIBLE_ITEMS + 1 && currentMenu->scrollOffset > 0)
      currentMenu->scrollOffset--;
  }

  draw_menuScreen(false);
}

static void button_down_handler (void)
{
  if (!currentMenu || currentMenu->itemCount == 0)
    return;

  if (currentMenu->currentSelection >= currentMenu->itemCount - 1)
  {
    currentMenu->currentSelection = 0;
    currentMenu->scrollOffset = 0;
  }
  else
  {
    currentMenu->currentSelection++;

    if (currentMenu->currentSelection > MAX_VISIBLE_ITEMS - 2 && currentMenu->scrollOffset < currentMenu->itemCount - MAX_VISIBLE_ITEMS)
      currentMenu->scrollOffset++;
  }

  draw_menuScreen(false);
}

static void button_enter_handler (void)
{
  if (currentMenu->itemCount == 0)
    return;

  MenuItem *item = &currentMenu->items[currentMenu->currentSelection];
  if (!item)
    return;

  if (item->prepareAction)
  {
    item->prepareAction();
  }

  if (item->submenu)
  {
    currentMenu = item->submenu;
    clear_position(currentMenu);
    draw_menuScreen(true);
  }

  if (item->postAction)
  {
    item->postAction();
  }
}

static void button_esc_handler (void)
{
  if (!currentMenu->parent)
    return;

  currentMenu = currentMenu->parent;

  draw_menuScreen(true);
}

static void idle_menu_handler (KeyEvent_t event)
{
  if (!rootMenu)
    return;

  switch (event.button)
  {
    case BTN_ENTER:
      currentMenu = rootMenu;
      clear_position(currentMenu);
      draw_menuScreen(true);
      break;
    default:
      break;
  }
}

static void alarm_info_menu_handler (KeyEvent_t event)
{
  if (!alarm_info_menu)
    return;

  switch (event.button)
  {
    case BTN_ESC:
      if (is_playing_handler())
	lcd_audio_notify(AUDIO_PREPARE_STOP, AUDIO_SD);

      if (alarm_info_menu->parent == NULL)
	return;
      currentMenu = alarm_info_menu->parent;
      draw_menuScreen(true);
      break;

    default:
      break;
  }
}

static void sinus_info_menu_handler (KeyEvent_t event)
{
  if (!sinusInfoMenu)
    return;

  switch (event.button)
  {
    case BTN_ESC:
      if (is_playing_handler())
	lcd_audio_notify(AUDIO_PREPARE_STOP, AUDIO_SIN);

      if (sinusInfoMenu->parent == NULL)
	return;
      currentMenu = sinusInfoMenu->parent;
      draw_menuScreen(true);
      break;

    default:
      break;
  }
}

static void maintenance_handle_button_press (KeyEvent_t event)
{
  if (!maintenanceMenu)
    return;

  switch (event.button)
  {
    case BTN_ESC:
      if (maintenanceMenu->parent == NULL)
	return;
      Password_Reset(false);
      currentMenu = maintenanceMenu->parent;
      draw_menuScreen(true);
      return;

    default:
      break;
  }
  handle_button_press(event);
}

void passwordMenu_handle_button_press(KeyEvent_t event)
{
  if ((!passwordMenu) && (currentMenu != passwordMenu)) return;

  switch(event.button)
  {
    case BTN_LEFT:     Password_Backspace();  return;

    case BTN_HASH:     Password_AddChar('#'); return;
    case BTN_ASTERISK: Password_AddChar('*'); return;

    case BTN_1:        Password_AddChar('1'); return;
    case BTN_2:        Password_AddChar('2'); return;
    case BTN_3:        Password_AddChar('3'); return;
    case BTN_4:        Password_AddChar('4'); return;
    case BTN_5:        Password_AddChar('5'); return;
    case BTN_6:        Password_AddChar('6'); return;
    case BTN_7:        Password_AddChar('7'); return;
    case BTN_8:        Password_AddChar('8'); return;
    case BTN_9:        Password_AddChar('9'); return;
    case BTN_0:        Password_AddChar('0'); return;
    case BTN_ENTER:
		      Password_Enter();
		      if (Password_IsCorrect())
		      {
			      currentMenu = maintenanceMenu;
			      draw_menuScreen(true);
		      }
		      return;
    case BTN_ESC:
      if (passwordMenu->parent == NULL)
      	return;
      Password_Reset(false);
      currentMenu = passwordMenu->parent;
      draw_menuScreen(true);
      return;
    default:
      break;
  }
}

static void volumeMenu_handle_button_press (KeyEvent_t event)
{
  if (!volumeMenu)
    return;

  switch (event.button)
  {
    case BTN_LEFT:
      Volume_DecreaseIndex();
      return;
    case BTN_RIGHT:
      Volume_IncreaseIndex();
      return;
    case BTN_1:        Volume_ChangeNumber(1); return;
    case BTN_2:        Volume_ChangeNumber(2); return;
    case BTN_3:        Volume_ChangeNumber(3); return;
    case BTN_4:        Volume_ChangeNumber(4); return;
    case BTN_5:        Volume_ChangeNumber(5); return;
    case BTN_6:        Volume_ChangeNumber(6); return;
    case BTN_7:        Volume_ChangeNumber(7); return;
    case BTN_8:        Volume_ChangeNumber(8); return;
    case BTN_9:        Volume_ChangeNumber(9); return;
    case BTN_0:        Volume_ChangeNumber(0); return;
    case BTN_ENTER:
      Volume_SendVolume();
      return;
    case BTN_ESC:
      if (volumeMenu->parent == NULL)
	return;
//      Password_Reset(false);
      currentMenu = volumeMenu->parent;
      draw_menuScreen(true);
      return;

    default:
      break;
  }
}

static void message_info_menu_handler (KeyEvent_t event)
{
  if (!messagePlayMenu)
    return;

  switch (event.button)
  {
    case BTN_ESC:
      if (is_playing_handler())
	lcd_audio_notify(AUDIO_PREPARE_STOP, AUDIO_SD);

      if (messagePlayMenu->parent == NULL)
	return;
      currentMenu = messagePlayMenu->parent;
      draw_menuScreen(true);
      break;

    default:
      break;
  }
}

//void sinus_info_menu_handler(KeyEvent_t event)
//{
//	if (!sinusInfoMenu) return;
//
//	switch(event.button)
//	{
//		case BTN_ESC:
//			if (sinusInfoMenu->parent != NULL) {
//				currentMenu = sinusInfoMenu->parent;
//				draw_menuScreen(true);
//			}
//			break;
//		default:
//				return;
//	}
//
//}

static void volume_control_handler (KeyEvent_t event)
{
  if (!currentMenu)
    return;

  AudioNotify_t audio_notify = {
  	  .event = AUDIO_VOLUME,
  	  .volume.handler = &volume_indicator_draw_bar,
  	  .priority = AUDIO_PRIORITY_LOW
  };

  switch (event.button)
  {
    case BTN_UP:
      audio_notify.volume.event = VOLUME_INCREASE;
      osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
      break;
    case BTN_DOWN:
      audio_notify.volume.event = VOLUME_DECREASE;
      osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
      break;
    case BTN_ESC:
      if (is_announcement_handler())
	lcd_audio_notify(AUDIO_STOP, AUDIO_MIC);

      currentMenu = currentMenu->parent;
      draw_menuScreen(true);
      break;
    default:
      return;
  }
}

static void clockMenu_handle_button_press (KeyEvent_t event)
{
  if (!clockMenu)
    return;

  switch (event.button)
  {
    case BTN_LEFT:
      DateTime_DecreaseIndex();
      return;
    case BTN_RIGHT:
      DateTime_IncreaseIndex();
      return;
    case BTN_1:        DateTime_ChangeNumber(1); return;
    case BTN_2:        DateTime_ChangeNumber(2); return;
    case BTN_3:        DateTime_ChangeNumber(3); return;
    case BTN_4:        DateTime_ChangeNumber(4); return;
    case BTN_5:        DateTime_ChangeNumber(5); return;
    case BTN_6:        DateTime_ChangeNumber(6); return;
    case BTN_7:        DateTime_ChangeNumber(7); return;
    case BTN_8:        DateTime_ChangeNumber(8); return;
    case BTN_9:        DateTime_ChangeNumber(9); return;
    case BTN_0:        DateTime_ChangeNumber(0); return;
    case BTN_ENTER:
      DateTime_Set();
      return;
    case BTN_ESC:
      if (clockMenu->parent == NULL)
	return;
      currentMenu = clockMenu->parent;
      draw_menuScreen(true);
      return;
    default:
      break;
  }
}

static void languageMenu_handle_button_press (KeyEvent_t event)
{
  if (!languageMenu || languageMenu->itemCount == 0)
    return;

  switch (event.button)
  {
    case BTN_ENTER:
      SetLanguage((Language) languageMenu->currentSelection);
      menu_init_language();
      draw_serial();
      draw_menuScreen(false);
      return;
    default:
      break;
  }
  handle_button_press(event);
}

static void confirmMenu_handle_button_press (KeyEvent_t event)
{
  if (!confirmMenu || confirmMenu->itemCount == 0)
     return;

   switch (event.button)
   {
    case BTN_ENTER:
      if(confirmMenu->currentSelection == 0)
      {
	confirm_handler();
      }
      else if(confirmMenu->currentSelection == 1)
      {
	button_esc_handler();
      }
     return;
    default:
      break;
   }
   handle_button_press(event);
}


static void loggingMenu_handle_button_press (KeyEvent_t event)
{
  if (!loggingMenu)
    return;

  switch (event.button)
  {
    case BTN_LEFT:
      Logging_Decrease();
//      Logging_ResetSeek();
      draw_menuScreen(true);
      return;
    case BTN_RIGHT:
      Logging_Increase();
//      Logging_ResetSeek();
      draw_menuScreen(true);
      return;
    case BTN_ESC:
      Logging_Close();
      break;
    default:
      break;
  }
  handle_button_press(event);
}

static void draw_textFilename (void)
{
  if (currentMenu->textFilename && currentMenu->textFilename[0] != '\0')
  {
    const char *src = currentMenu->textFilename;
    FontDef *font = &Font_16x26;
    Language lang = GetLanguage();

    size_t max_chars = (hx8357_get_width() - (MENU_BASE_X * 2)) / font->width;

    char buffer[FF_MAX_LFN];
    size_t src_len = strlen(src);

    if (src_len <= max_chars)
    {
      strcpy(buffer, src);
    }
    else
    {
      if (lang == LANG_EN)
      {
	memcpy(buffer, src, max_chars);
	buffer[max_chars] = '\0';
      }
      else if (lang == LANG_HE)
      {
	const char *start = src + (src_len - max_chars);
	memcpy(buffer, start, max_chars);
	buffer[max_chars] = '\0';
      }
    }

    hx8357_write_alignedX_string(0, TEXT_Y_POS, buffer, font, COLOR_MAGENTA, COLOR_BLACK, ALIGN_CENTER);
  }
}

static void check_playing_and_stop ()
{
  if (is_playing_handler() || is_announcement_handler())
  {
    lcd_audio_notify(AUDIO_STOP, AUDIO_CURRENT_TYPE);
  }
}

static void lcd_audio_notify(AudioEvent_t event, AudioType_t type)
{
  AudioNotify_t audio_notify = {
      .event = event,
      .sample.type = type,
      .priority = AUDIO_PRIORITY_LOW };

  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
}

static void lcd_notify_start_task_low (AudioType_t type,
				  SinTask_t task,
				  const char *name)
{
  AudioNotify_t audio_notify = {
      .event = AUDIO_START,
      .sample.type = type,
      .sample.sin_task = task,
      .sample.filename = name,
      .priority = AUDIO_PRIORITY_LOW };

  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
}

static void lcd_notify_arming (bool val)
{
  AudioNotify_t audio_notify = { .event = AUDIO_ARMIG, .priority = AUDIO_PRIORITY_LOW, .arming = val };

  osMessageQueuePut(xAudioQueueHandle, &audio_notify, 0, 10);
}

void lcd_is_playing (state_handler_t h)
{
  is_playing_handler = h;
}
void lcd_is_stoped (state_handler_t h)
{
  is_stoped_handler = h;
}
void lcd_is_recording (state_handler_t h)
{
  is_recording_handler = h;
}
void lcd_is_announcement (state_handler_t h)
{
  is_announcement_handler = h;
}
void lcd_is_motorola (state_handler_t h)
{
  is_motorola_handler = h;
}
void lcd_is_arming (state_handler_t h)
{
  is_arming_handler = h;
}

void lcd_volume_indicator (volume_indicator h)
{
  lcd_volume_indicator_handler = h;
}
