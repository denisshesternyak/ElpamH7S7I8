#ifndef INC_APP_FREERTOS_H_
#define INC_APP_FREERTOS_H_

#include "cmsis_os2.h"

extern osMessageQueueId_t xLoggerQueueHandle;
extern osSemaphoreId_t LoggerBinarySemHandle;
extern osMutexId_t LoggerMutexHandle;

extern osMessageQueueId_t xLCDQueueHandle;
extern osSemaphoreId_t LcdBinarySemHandle;

#endif /* INC_APP_FREERTOS_H_ */
