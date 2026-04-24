#ifndef INC_LOGGER_LOGGER_H_
#define INC_LOGGER_LOGGER_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#define LOG_BUFFER_SIZE    256
#define LOG_QUEUE_SIZE     32
#define LOG_MAX_RETRIES    3

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} LogLevel_t;

typedef struct {
    char *buffer;
    LogLevel_t level;
    uint16_t len;
    uint8_t retry_count;
} LogMessage_t;

void logger_init(void);
void logger_process(LogMessage_t *msg);

void logger_set_level(LogLevel_t level);
LogLevel_t logger_get_level(void);

void logger_msg(LogLevel_t level, const char *fmt, ...);

#define LOG_DEBUG(...)  logger_msg(LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(fmt, ...)   logger_msg(LOG_INFO,  fmt, ##__VA_ARGS__)
#define LOG_WARN(...)   logger_msg(LOG_WARN,  __VA_ARGS__)
#define LOG_ERROR(...)  logger_msg(LOG_ERROR, __VA_ARGS__)
#define LOG_FATAL(...)  logger_msg(LOG_FATAL, __VA_ARGS__)

#endif /* INC_LOGGER_LOGGER_H_ */
