#include "logger.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "app_freertos.h"
#include "rtc.h"

#if !USED_SD
#define LOGGER_HANDLER	&huart1
static volatile bool uart_error;
#else
#define BACKUP_NAME_FORMAT  	"%s/backup_%02d.txt"
#define MAX_BACKUP_FILES    	99

Logger_t logger;

static char w_src_path[128];
static WCHAR w_dest_path[128];
#endif

#define TIME_STAMP_SIZE 24
static const char *TIME_FORMAT = "[%02d:%02d:%02d.%03ld] %s %s";

static LogLevel_t current_log_level = LOG_DEBUG;

static char logger_buffer_pool[LOG_QUEUE_SIZE][LOG_BUFFER_SIZE] __attribute__((section(".extram")));
static char logger_buffer_temp[LOG_BUFFER_SIZE] __attribute__((section(".extram")));

static volatile size_t write_index = 0;
static volatile size_t read_index = 0;
static volatile size_t free_count = LOG_QUEUE_SIZE;

static const char *const level_strings[] = {
    "[DEBUG]",
    "[INFO]",
    "[WARN]",
    "[ERROR]",
    "[FATAL]" };

#if !USED_SD
  static void logger_uart_tx_complete_callback (UART_HandleTypeDef *huart)
  {
    osSemaphoreRelease(LoggerBinarySemHandle);
  }
#else
static uint8_t logger_get_last_backup_number (void);
static void logger_rotate (void);
static void logger_check_log_file (void);
static void logger_write (const uint8_t *data, uint16_t len);
#endif

static void logger_format_message (char *temp, LogMessage_t *msg);

static void logger_init_msg()
{
  RTC_TimeTypeDef sTime = { 0 };
  RTC_DateTypeDef sDate = { 0 };

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  LOG_INFO("**********************");
  LOG_INFO("System run: [%02d/%02d/%02d]", sDate.Date, sDate.Month, sDate.Year);
  LOG_INFO("**********************\r\n\r\n");
}

void logger_init (void)
{
#if !USED_SD
  usart_register_tx_callback(LOGGER_HANDLER, logger_uart_tx_complete_callback);
  uart_error = false;
#else
  if (!sdfs_state.is_mounted)
    return;

  memset(&logger, 0, sizeof(Logger_t));

  sdfs_create_dir(LOG_DIR_PATH_U);
  sdfs_create_dir(LOG_BACKUP_DIR_PATH_U);

  logger_check_log_file();

  logger.is_log_open = sdfs_open_from_last_entry(&logger.log_file, LOG_FILE_PATH_U);
#endif

  logger_init_msg();
}

static char* buffer_pool_alloc (void)
{
  char *buffer = NULL;

  if (LoggerMutexHandle != NULL)
  {
    if (osMutexAcquire(LoggerMutexHandle, 100) == osOK)
    {
      if (free_count > 0)
      {
	buffer = logger_buffer_pool[write_index];
	write_index = (write_index + 1) % LOG_QUEUE_SIZE;
	free_count--;
      }
      osMutexRelease(LoggerMutexHandle);
    }
  }

  return buffer;
}

static void buffer_pool_free (char *buffer)
{
  if (buffer == NULL || LoggerMutexHandle == NULL)
    return;

  if (osMutexAcquire(LoggerMutexHandle, 100) == osOK)
  {
    if (buffer >= logger_buffer_pool[0] && buffer < logger_buffer_pool[LOG_QUEUE_SIZE])
    {
      read_index = (read_index + 1) % LOG_QUEUE_SIZE;
      free_count++;
    }
    osMutexRelease(LoggerMutexHandle);
  }
}

#if USED_SD
void logger_close(void)
{
  sdfs_close_file(&logger.log_file);
  logger.is_log_open = false;
}

static uint8_t logger_get_last_backup_number (void)
{
  uint8_t last;

  for (last = 1; last < MAX_BACKUP_FILES; last++)
  {
    snprintf(w_src_path, sizeof(w_src_path), BACKUP_NAME_FORMAT, LOG_BACKUP_DIR_PATH, last);

    sdfs_convert_eom2uni(w_src_path, w_dest_path);

    if (!sdfs_is_file_exist((const TCHAR*) w_dest_path))
      break;
  }
  return last;
}

static void logger_rotate (void)
{
  if (logger.is_log_open)
  {
    logger_close();
  }

  uint8_t next = logger_get_last_backup_number();

  if (next > MAX_BACKUP_FILES)
    next = 1;

  snprintf(w_src_path, sizeof(w_src_path), BACKUP_NAME_FORMAT,
  LOG_BACKUP_DIR_PATH, next);

  sdfs_convert_eom2uni(w_src_path, w_dest_path);

  sdfs_send_to_backup(LOG_FILE_PATH_U, (const TCHAR*) w_dest_path);
}

static void logger_check_log_file ()
{
  if (sdfs_is_file_exist_not_null(LOG_FILE_PATH_U))
  {
    logger_rotate();
  }
}

static void logger_write (const uint8_t *data, uint16_t len)
{
  if (!logger.is_log_open)
    return;

  sdfs_write_data(&logger.log_file, data, len);
}
#endif

void logger_msg (LogLevel_t level, const char *fmt, ...)
{
  if (level < current_log_level)
    return;

#if USED_SD
  if (!sdfs_state.is_mounted)
    return;
#endif

  char *buffer = buffer_pool_alloc();
  if (!buffer)
  {
    return;
  }

  va_list args;
  va_start(args, fmt);
  int msg_len = vsnprintf(buffer, LOG_BUFFER_SIZE, fmt, args);
  va_end(args);

  if (msg_len < 0)
    msg_len = 0;
  if (msg_len > LOG_BUFFER_SIZE - 1)
    msg_len = LOG_BUFFER_SIZE - 1;

  LogMessage_t msg = {
      .buffer = buffer,
      .level = level,
      .len = msg_len,
      .retry_count = 0 };

  if (osMessageQueuePut(xLoggerQueueHandle, &msg, 0, 0) != osOK)
  {
    buffer_pool_free(msg.buffer);
    msg.buffer = NULL;
  }
}

void log_set_level (LogLevel_t level)
{
  current_log_level = level;
}

LogLevel_t log_get_level (void)
{
  return current_log_level;
}

void logger_process (LogMessage_t *msg)
{
  if (!msg || msg->len == 0)
    return;

#if !USED_SD
  if (osSemaphoreAcquire(LoggerBinarySemHandle, 1000) != osOK || uart_error)
  {
    HAL_UART_Abort(LOGGER_HANDLER);
    uart_error = false;
  }
#else
  if (!sdfs_state.is_mounted)
    return;
#endif

  logger_format_message(logger_buffer_temp, msg);

#if !USED_SD
  HAL_UART_Transmit_DMA(LOGGER_HANDLER, (uint8_t*) msg->buffer, msg->len);
#else
  logger_write((uint8_t*) msg->buffer, msg->len);
#endif

  buffer_pool_free(msg->buffer);
  msg->buffer = NULL;
}

static void logger_format_message (char *temp, LogMessage_t *msg)
{
  RTC_TimeTypeDef sTime = { 0 };

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

  uint32_t milliseconds = 1000 * (sTime.SecondFraction - sTime.SubSeconds) / (sTime.SecondFraction + 1);

  memcpy(temp, msg->buffer, msg->len);
  temp[msg->len] = '\0';

  int total_len = snprintf(msg->buffer, LOG_BUFFER_SIZE, TIME_FORMAT, sTime.Hours, sTime.Minutes, sTime.Seconds, milliseconds, level_strings[msg->level], temp);

  if (total_len > 0 && msg->buffer[total_len - 1] != '\n')
  {
    if (total_len + 2 < LOG_BUFFER_SIZE)
    {
      msg->buffer[total_len++] = '\r';
      msg->buffer[total_len++] = '\n';
      msg->buffer[total_len] = '\0';
    }
  }
  else
  {
    msg->buffer[total_len] = '\0';
  }

  msg->len = total_len;
}
