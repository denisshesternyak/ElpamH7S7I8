/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "extmem_manager.h"
#include "flash.h"
#include "gpdma.h"
#include "i2c.h"
#include "sbs.h"
#include "sdmmc.h"
#include "usart.h"
#include "xspi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "fw_update.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//#define SELFTEST

#ifdef SELFTEST
uint8_t data_tx[] = "Hello world from H7s7!";
uint8_t data_rx[60];

static bool test_nor ()
{
  size_t len = sizeof(data_tx) - 1;
  uint32_t nor_base = 0;
  uint32_t offset = 0x01FFF000;

  EXTMEM_GetMapAddress(EXTMEMORY_1, &nor_base);
  printf("NOR mapped at 0x%08lX\r\n", nor_base);

  EXTMEM_EraseSector(EXTMEMORY_1, offset, 4096);
  EXTMEM_Write(EXTMEMORY_1, offset, data_tx, len);
  EXTMEM_Read(EXTMEMORY_1, offset, data_rx, len);

  EXTMEM_MemoryMappedMode(EXTMEMORY_1, EXTMEM_ENABLE);
//	memcpy((uint8_t*) nor_base, data_tx, len);
//	memcpy(data_rx, (uint8_t*) nor_base, len);
  EXTMEM_MemoryMappedMode(EXTMEMORY_1, EXTMEM_DISABLE);

  uint8_t cmp = memcmp(data_tx, data_rx, len) == 0;
  printf("NOR write/read test %s\r\n", cmp ? "PASSED" : "FAILED");
  return cmp;
}

static bool test_psram ()
{
  size_t len = sizeof(data_tx) - 1;
  uint32_t psram_base = 0;
  uint32_t offset = 0x01FFF000;

  EXTMEM_GetMapAddress(EXTMEMORY_2, &psram_base);
  printf("PSRAM mapped at 0x%08lX\r\n", psram_base);

  EXTMEM_MemoryMappedMode(EXTMEMORY_2, EXTMEM_ENABLE);

  uint8_t* psram_ptr = (uint8_t*)(psram_base + offset);
  memset(psram_ptr, 0, 0x1000);
  memcpy(psram_ptr, data_tx, len);
  memcpy(data_rx, psram_ptr, len);

  EXTMEM_MemoryMappedMode(EXTMEMORY_2, EXTMEM_DISABLE);

  uint8_t cmp = memcmp(data_tx, data_rx, len) == 0;
  printf("PSRAM write/read test %s\r\n", cmp ? "PASSED" : "FAILED");
  return cmp;
}
#endif
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_FLASH_Init();
  MX_SBS_Init();
  MX_USART1_UART_Init();
  MX_XSPI1_Init();
  MX_XSPI2_Init();
  MX_UART5_Init();
  MX_UART7_Init();
  MX_UART4_Init();
  MX_SDMMC1_SD_Init();
  MX_I2C3_Init();
  MX_CRC_Init();
  MX_EXTMEM_MANAGER_Init();
  /* USER CODE BEGIN 2 */
#ifdef SELFTEST
  if(!(test_nor() && test_psram()))
  {
    Error_Handler();
  }
#endif

  printf("Bootloader running\r\n");
//  fw_process();
  /* USER CODE END 2 */

  /* Launch the application */
  if (BOOT_OK != BOOT_Application())
  {
    Error_Handler();
  }
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL1.PLLM = 2;
  RCC_OscInitStruct.PLL1.PLLN = 50;
  RCC_OscInitStruct.PLL1.PLLP = 1;
  RCC_OscInitStruct.PLL1.PLLQ = 1;
  RCC_OscInitStruct.PLL1.PLLR = 2;
  RCC_OscInitStruct.PLL1.PLLS = 2;
  RCC_OscInitStruct.PLL1.PLLT = 2;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL2.PLLM = 2;
  RCC_OscInitStruct.PLL2.PLLN = 50;
  RCC_OscInitStruct.PLL2.PLLP = 4;
  RCC_OscInitStruct.PLL2.PLLQ = 4;
  RCC_OscInitStruct.PLL2.PLLR = 2;
  RCC_OscInitStruct.PLL2.PLLS = 3;
  RCC_OscInitStruct.PLL2.PLLT = 2;
  RCC_OscInitStruct.PLL2.PLLFractional = 0;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK4|RCC_CLOCKTYPE_PCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    HAL_GPIO_TogglePin(LED_3_GPIO_Port, LED_3_Pin);
    HAL_Delay(200);
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
