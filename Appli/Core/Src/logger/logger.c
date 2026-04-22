#include "logger.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "app_freertos.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"

static LogLevel_t current_log_level = LOG_DEBUG;
static volatile bool uart_error;

static char logger_buffer_pool[LOG_QUEUE_SIZE][LOG_BUFFER_SIZE] __attribute__((section(".extram")));
static volatile size_t write_index = 0;
static volatile size_t read_index = 0;
static volatile size_t free_count = LOG_QUEUE_SIZE;

static const char *const level_strings[] = {
    "[DEBUG]",
    "[INFO]",
    "[WARN]",
    "[ERROR]",
    "[FATAL]" };

static void logger_format_message (LogMessage_t *msg,
				   LogLevel_t level,
				   const char *fmt,
				   va_list args);

static void logger_uart_tx_complete_callback (void)
{
  osSemaphoreRelease(LoggerBinarySemHandle);
}

static void logger_uart_error_callback (void)
{
  uart_error = true;
  HAL_UART_Abort(&huart1);
}

void logger_init (void)
{
  usart_register_tx_callback(&huart1, logger_uart_tx_complete_callback);

  uart_error = false;
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

void logger_msg (LogLevel_t level, const char *fmt, ...)
{
  if (level < current_log_level)
    return;

  char *buffer = buffer_pool_alloc();
  if (buffer == NULL)
  {
    return;
  }

  LogMessage_t msg = { .buffer = buffer };

  va_list args;
  va_start(args, fmt);
  logger_format_message(&msg, level, fmt, args);
  va_end(args);

  if (osMessageQueuePut(xLoggerQueueHandle, &msg, 100, 0) != osOK)
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

  while (msg->retry_count < LOG_MAX_RETRIES)
  {
    if (osSemaphoreAcquire(LoggerBinarySemHandle, 1000) != osOK || uart_error)
    {
      HAL_UART_Abort(&huart1);
      uart_error = false;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(&huart1, (uint8_t*) msg->buffer, msg->len);

    if (status == HAL_OK)
    {
      break;
    }
    else
    {
      msg->retry_count++;
      osDelay(10);
    }
  }

  if (msg->retry_count >= LOG_MAX_RETRIES)
  {
    HAL_UART_Abort(&huart1);
    uart_error = false;
    buffer_pool_free(msg->buffer);
    msg->buffer = NULL;
  }
}

static void logger_format_message (LogMessage_t *msg,
				   LogLevel_t level,
				   const char *fmt,
				   va_list args)
{
  uint32_t tick = osKernelGetTickCount();

  int len = snprintf(msg->buffer, LOG_BUFFER_SIZE, "%s [%lu] ", level_strings[level], tick);
  int msg_len = vsnprintf(msg->buffer + len, LOG_BUFFER_SIZE - len, fmt, args);
  if (msg_len < 0)
    msg_len = 0;

  size_t total_len = len + msg_len;

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
  msg->retry_count = 0;
}
