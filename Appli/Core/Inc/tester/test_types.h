#ifndef INC_TESTER_TEST_TYPES_H_
#define INC_TESTER_TEST_TYPES_H_

typedef enum
{
  TEST_IDLE,
  TEST_START_SINUS,
  TEST_START_SILENT,
  TEST_START_BATT,
  TEST_START_AMP,
  TEST_START_DRV
} TestEvent_t;

typedef struct
{
  TestEvent_t event;
  int16_t *data;
} TestADMessage_t;

#endif /* INC_TESTER_TEST_TYPES_H_ */
