#include "tests.h"

#include <string.h>
#include <stdio.h>
#include "fatfs.h"
#include "usart.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "events.h"
#include "sdfs.h"
#include "logger.h"
#include "lcd_menu.h"
#include "rs232.h"
#include "command_dispatcher.h"
#include "system_status.h"
#include "audio.h"
#include "keyboard.h"
#include "analog.h"
#include "usart.h"

static void print_msg(const char *msg, bool is_passed);
static uint8_t msg_rx[20];
static void uart_complete_callback (void);
static bool is_rx_tc = false;

static bool test_i2c1()
{
  printf("Test keyboard...\r\n");
  osDelay(10);

  HAL_GPIO_WritePin(KEYPAD_RST_GPIO_Port, KEYPAD_RST_Pin, GPIO_PIN_SET);
  osDelay(10);

  uint8_t reg = 0x1D;
  uint8_t data;
  uint8_t data_rx;

  HAL_StatusTypeDef status;

  status = HAL_I2C_Mem_Read(&hi2c1, 0x68, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);

  data |= 1;

  status = HAL_I2C_Mem_Write(&hi2c1, 0x68, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
  status = HAL_I2C_Mem_Read(&hi2c1, 0x68, reg, I2C_MEMADD_SIZE_8BIT, &data_rx, 1, 100);

  bool res = (status == HAL_OK) && (data == data_rx);

  HAL_GPIO_WritePin(KEYPAD_RST_GPIO_Port, KEYPAD_RST_Pin, GPIO_PIN_RESET);
  printf("Test keyboard completed: %s\r\n", res ? "PASSED" : "FAILURE");
  osDelay(10);

  return res;
}

static bool test_i2c3()
{
  printf("Test eeprom...\r\n");
  osDelay(10);

  uint8_t data_tx[] = "Hello EEPROM";
  uint8_t data_rx[20];
  const uint8_t len = sizeof(data_tx);

  HAL_StatusTypeDef res;

  printf("Write to eeprom: %s\r\n", data_tx);
  osDelay(10);

  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_RESET);
  res = HAL_I2C_Mem_Write(&hi2c3, 0xA0, 0x0000, I2C_MEMADD_SIZE_16BIT, data_tx, len, 100);
  HAL_GPIO_WritePin(EP_WP_GPIO_Port, EP_WP_Pin, GPIO_PIN_SET);

  while (res == HAL_OK && HAL_I2C_IsDeviceReady(&hi2c3, 0xA0, 3, 100) != HAL_OK);

  res = HAL_I2C_Mem_Read(&hi2c3, 0xA0, 0x0000, I2C_MEMADD_SIZE_16BIT, data_rx, len, 100);
  if(res != HAL_OK)
      return false;

  printf("Read from eeprom: %s\r\n", data_rx);
  osDelay(10);

  bool cmp = memcmp(data_tx, data_rx, len) == 0;
  printf("Test eeprom completed: %s\r\n", cmp ? "PASSED" : "FAILURE");
  osDelay(10);

  return cmp;
}

static bool test_uSD()
{
  if(!sdfs_is_detected())
    return false;

  printf("Test uSD...\r\n");
  osDelay(10);

  const uint8_t wtext[] = "This is STM32 working with FatFs uSD";
  uint8_t rtext[100];

  FRESULT res;
  uint32_t byteswritten, bytesread;
  bool cmp = false;
  const char* file_name = "STM32.TXT";

  WCHAR w_path[128];
  int j = 0;
  while (file_name[j] != 0 && j < 128)
  {
    w_path[j] = ff_oem2uni(file_name[j], FF_CODE_PAGE);
    j++;
  }
  w_path[j] = '\0';

  if(!sdfs_state.is_mounted)
    res = f_mount(&SDFatFs, (TCHAR*)u"/", 1);
  else
    res = FR_OK;

  if(res == FR_OK)
  {
    if(f_open(&SDFile, (TCHAR*) w_path, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    {
      printf("Write to uSD: %s\r\n", wtext);
      osDelay(10);

      res = f_write(&SDFile, (const void *)wtext, sizeof(wtext), (void *)&byteswritten);

      if((byteswritten > 0) && (res == FR_OK))
      {
	f_close(&SDFile);

	if(f_open(&SDFile, (TCHAR*) w_path, FA_READ) == FR_OK)
	{
	  res = f_read(&SDFile, ( void *)rtext, sizeof(rtext), (void *)&bytesread);

	  printf("Read to uSD: %s\r\n", rtext);
	  osDelay(10);

	  if((bytesread > 0) && (res == FR_OK))
	  {
	    f_close(&SDFile);
	    cmp = bytesread == byteswritten;
	  }
	}
      }
    }
  }

  printf("Test uSD card completed: %s\r\n", cmp ? "PASSED" : "FAILURE");
  osDelay(10);

  return cmp;
}

static bool test_rtc()
{
  printf("Test RTC...\r\n");
  osDelay(10);

  RTC_TimeTypeDef sTime = { 0 };
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  uint32_t milliseconds_before = 1000 * (sTime.SecondFraction - sTime.SubSeconds) / (sTime.SecondFraction + 1);
  printf("Get milliseconds: %ldms and wait 100ms...\r\n", milliseconds_before);

  osDelay(100);

  RTC_TimeTypeDef sTime2 = { 0 };
  HAL_RTC_GetTime(&hrtc, &sTime2, RTC_FORMAT_BIN);
  uint32_t milliseconds_after = 1000 * (sTime2.SecondFraction - sTime2.SubSeconds) / (sTime2.SecondFraction + 1);

  printf("Get milliseconds: %ldms\r\n", milliseconds_after);
  osDelay(10);

  bool time_changed = milliseconds_after != milliseconds_before;

  printf("Difference before/after: %ldms\r\n", milliseconds_after - milliseconds_before);
  osDelay(10);
  printf("Test RTC completed: %s\r\n", time_changed ? "PASSED" : "FAILURE");
  osDelay(10);

  return time_changed;
}

static bool test_codec()
{
  printf("Test Tx CODEC...\r\n");
  osDelay(10);

  HAL_GPIO_WritePin(CODEC_RESET_GPIO_Port, CODEC_RESET_Pin, GPIO_PIN_RESET);
  osDelay(50);
  HAL_GPIO_WritePin(CODEC_RESET_GPIO_Port, CODEC_RESET_Pin, GPIO_PIN_SET);
  osDelay(200);

  uint8_t reg = 0x00;
  uint8_t value = 0x00;

  uint8_t data_tx[2];
  data_tx[0] = reg << 1 | 0x00;
  data_tx[1] = value;

  HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, GPIO_PIN_RESET);
  HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi4, data_tx, 2, 100);
  HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, GPIO_PIN_SET);

  bool cmp = status == HAL_OK;

  printf("Test Tx CODEC completed: %s\r\n", cmp ? "PASSED" : "FAILURE");
  osDelay(10);

  return cmp;
}

static void uart_complete_callback (void)
{
  is_rx_tc = true;
}

static bool test_rs232a()
{
  printf("Test T/R RS232a...\r\n");
  osDelay(10);

  usart_register_rx_callback(&huart7, uart_complete_callback);

  uint8_t msg[] = "Test msg RS232a";
  uint16_t len = sizeof(msg);
  is_rx_tc = false;
  memset(msg_rx, '\0', 20);

  HAL_UART_Receive_IT(&huart7, msg_rx, len);
  HAL_StatusTypeDef status = HAL_UART_Transmit(&huart7, msg, len, HAL_MAX_DELAY);

  printf("Sent message \"%s\" to RS232a\r\n", msg);

  uint8_t i = 100;
  while(i-- && !is_rx_tc)
  {
    osDelay(1);
  }

  if(is_rx_tc)
  {
    printf("Received from RS232a: %s\r\n", msg_rx);
  }

  bool res = (status == HAL_OK) && (memcmp(msg, msg_rx, len) == 0);

  printf("Test RS232a completed: %s\r\n", res ? "PASSED" : "FAILURE");
  osDelay(10);

  usart_unregister_callback(&huart5);

  return res;
}

static bool test_rs232b()
{
  printf("Test T/R RS232b...\r\n");
  osDelay(10);

  usart_register_rx_callback(&huart4, uart_complete_callback);

  uint8_t msg[] = "Test msg RS232b";
  uint16_t len = sizeof(msg);
  is_rx_tc = false;
  memset(msg_rx, '\0', 20);

  HAL_UART_Receive_IT(&huart4, msg_rx, len);
  HAL_StatusTypeDef status = HAL_UART_Transmit(&huart4, msg, len, HAL_MAX_DELAY);

  printf("Sent message \"%s\" to RS232b\r\n", msg);

  uint8_t i = 100;
  while(i-- && !is_rx_tc)
  {
    osDelay(1);
  }

  if(is_rx_tc)
  {
    printf("Received from RS232b: %s\r\n", msg_rx);
  }

  bool res = (status == HAL_OK) && (memcmp(msg, msg_rx, len) == 0);

  printf("Test RS232b completed: %s\r\n", res ? "PASSED" : "FAILURE");
  osDelay(10);

  usart_unregister_callback(&huart5);

  return res;
}

static bool test_rs485()
{
  printf("Test T/R RS485...\r\n");
  osDelay(10);

  usart_register_rx_callback(&huart5, uart_complete_callback);

  uint8_t msg[] = "Test msg RS485";
  uint16_t len = sizeof(msg);
  is_rx_tc = false;
  memset(msg_rx, '\0', 20);

  HAL_UART_Receive_IT(&huart5, msg_rx, len);
  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
  HAL_StatusTypeDef status = HAL_UART_Transmit(&huart5, msg, len, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);

  printf("Sent message \"%s\" to RS485\r\n", msg);

  uint8_t i = 100;
  while(i-- && !is_rx_tc)
  {
    osDelay(1);
  }

  if(is_rx_tc)
  {
    printf("Received from RS485: %s\r\n", msg_rx);
  }

  bool res = (status == HAL_OK) && (memcmp(msg, msg_rx, len) == 0);

  printf("Test RS485 completed: %s\r\n", res ? "PASSED" : "FAILURE");
  osDelay(10);

  usart_unregister_callback(&huart5);

  return res;
}

bool test_board(void)
{
  HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_SET);

  hx8357_write_alignedX_string(0, 0, "### PERIPHERAL TESTS ###", &Font_16x26, COLOR_MAGENTA, COLOR_BLACK, ALIGN_CENTER);

  printf("Starting peripheral tests...\r\n");

  print_msg("NOR memory: ", true);
  print_msg("PSRAM memory: ", true);
  print_msg("EEPROM: ", test_i2c3());
  print_msg("uSD: ", test_uSD());
  print_msg("RTC: ", test_rtc());
  print_msg("Tx Codec: ", test_codec());
  print_msg("T/R keyboard: ", test_i2c1());
  print_msg("T/R RS232a: ", test_rs232a());
  print_msg("T/R RS232b: ", test_rs232b());
  print_msg("T/R RS485: ", test_rs485());

  osDelay(10000);

  hx8357_fill_screen(COLOR_BLACK);

  HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_RESET);

  return true;
}

static void print_msg(const char *msg, bool is_passed)
{
  FontDef *font = &Font_11x18;
  uint8_t x = 20;
  static uint8_t y = 40;

  char str[64];

  snprintf(str, 64, msg);
  hx8357_write_alignedX_string(x, y, str, font, COLOR_WHITE, COLOR_BLACK, ALIGN_LEFT);

  snprintf(str, 64, is_passed ? "PASSED" : "FAILURE");
  hx8357_write_alignedX_string(x, y, str, font, is_passed ? COLOR_GREEN : COLOR_RED, COLOR_BLACK, ALIGN_RIGHT);

  y += font->height + 5;
}
