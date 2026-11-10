#ifndef INC_TESTER_TEST_TYPES_H_
#define INC_TESTER_TEST_TYPES_H_

typedef enum
{
  TEST_IDLE,
  TEST_START,
  TEST_STOP
} TestEvent_t;

typedef struct
{
  TestEvent_t event;
  int16_t *data;
} TestADMessage_t;

#endif /* INC_TESTER_TEST_TYPES_H_ */
