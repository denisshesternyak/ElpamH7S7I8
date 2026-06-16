#ifndef INC_DEFINES_H_
#define INC_DEFINES_H_

#define ROOT_DIR_PATH 		"/"
#define ALARMS_DIR_PATH 	"/alarms"
#define MESSAGES_DIR_PATH 	"/messages"
#define FIRMWARE_DIR_PATH 	"/firmware"
#define BOOT_DIR_PATH 		"/boot"
#define LOG_DIR_PATH 		"/log"
#define LOG_BACKUP_DIR_PATH 	"/log/backup"
#define LOG_FILE_PATH 		"/log/log.txt"

#define ROOT_DIR_PATH_U 	((const TCHAR *)u"/")
#define ALARMS_DIR_PATH_U 	((const TCHAR *)u"/alarms")
#define MESSAGES_DIR_PATH_U 	((const TCHAR *)u"/messages")
#define FIRMWARE_DIR_PATH_U 	((const TCHAR *)u"/firmware")
#define BOOT_DIR_PATH_U 	((const TCHAR *)u"/boot")
#define LOG_DIR_PATH_U 		((const TCHAR *)u"/log")
#define LOG_BACKUP_DIR_PATH_U 	((const TCHAR *)u"/log/backup")
#define LOG_FILE_PATH_U 	((const TCHAR *)u"/log/log.txt")

#define USED_SD 		0
#define MAINTENANCE_PASSWORD 	"9999"

#define VER_MAJOR  		0
#define VER_MINOR  		22

#define LCD_USART_TEXT_Y        300
#define LCD_USART_TEXT_X        5
#define LCD_USART_TEXT_FONT    	&Font_11x18


//#define MAX_VOLUME_CODEC    0
//#define MIN_VOLUME_CODEC    200
#define MAX_VOLUME_CODEC    	0
#define MIN_VOLUME_CODEC    	40

#define MAX_VOLUME_PLAYBACK    	150
#define MIN_VOLUME_PLAYBACK    	0
#define CUR_VOLUME_PLAYBACK    	150

#define MAX_VOLUME          	122
#define MIN_VOLUME          	80
#define VOLUME_STEP         	3
#define DEF_VALUE_VOLUME    	80

#define NUM_VALID_LEVELS    	15

#define NUM_VOLUME_BARS     	NUM_VALID_LEVELS

#define MAX_MENU_ITEMS 		25

#define INACTIVITY_TIMEOUT_MS 	60000
#define BACKLIGHT_TIMEOUT_MS 	10000
#define ARMING_TIME  		10000
#define ANNOUNCEMENT_TIME  	180000
#define RECORDING_TIME		10000

#endif /* INC_DEFINES_H_ */
