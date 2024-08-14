/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sh1106.h"
#include "sh1106_fonts.h"
#include "sh1106_bitmap.h"
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
//array for heartbeats
uint16_t val[]={101, 101, 104, 104, 106, 103, 107, 106, 105, 104, 109, 112, 119, 122, 121, 123, 125, 125, 129, 128, 127, 122,
        121, 123, 115, 120, 113, 119, 111, 112, 109, 107, 104, 107, 102, 107, 105, 98, 102, 102, 99, 98, 97, 100, 101,
        96, 93, 92, 91, 81, 77, 76, 72, 64, 42, 38, 37, 36, 32, 17, 11, 10, 9, 16, 10, 9, 1, 4, 0, 8, 80, 79, 78, 77,
        72, 71, 112, 111, 110, 109, 108, 107, 106, 105, 120, 103, 100, 99, 98, 97, 162, 161, 124, 123, 160, 159, 156,
        155, 154, 224, 223, 222, 221, 214, 213, 212, 204, 203, 255, 254, 253, 252, 251, 250, 249, 247, 246, 245, 220,
        219, 218, 217, 282, 281, 264, 263, 256, 243, 242, 241, 240, 239, 238, 237, 236, 235, 234, 233, 295, 294, 280,
        279, 278, 277, 276, 275, 274, 268, 267, 264, 263, 348, 347, 374, 373, 372, 371, 367, 366, 365, 368, 367, 346,
        343, 470, 454, 453, 452, 451, 450, 449, 474, 456, 455, 454, 453, 452, 451, 450, 449, 448, 447, 446, 445, 444,
        443, 432, 431, 430, 428, 427, 426, 425, 424, 423, 422, 421, 326, 325, 324, 298, 297, 295, 294, 272, 271, 270,
        269, 295, 294, 267, 200, 199, 256, 255, 254, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 236, 177,
        176, 175, 167, 166, 165, 164, 163, 165, 164, 163, 162, 161, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114,
        113, 110, 109, 108, 107, 106, 105, 88, 87, 86, 85, 84, 53, 65, 64, 61, 64, 63, 75, 79, 74, 72, 77, 86, 85, 81,
        80, 94, 92, 91, 95, 101, 95, 94, 93, 106, 106, 106, 106, 104, 104, 104, 104, 104, 104, 104, 104, 103, 105, 103,
        105, 103, 105, 103, 103, 103, 103, 106, 106, 105, 106, 106, 106, 106, 106, 104, 104, 103, 103, 109, 108, 101,
        105, 117, 113, 117, 116, 111, 110, 119, 113, 120, 136, 135, 124, 129, 128, 153, 154, 156, 159, 158, 162, 167,
        177, 176, 175, 174, 177, 178, 185, 180, 183, 186, 187, 185, 186, 184, 182, 182, 179, 175, 179, 177, 176, 174,
        173, 171, 170, 171, 169, 166, 164, 170, 165, 163, 164, 160, 157, 157, 153, 155, 151, 148, 147, 149, 151, 145,
        149, 144, 142, 144, 143, 137, 136, 139, 138, 137, 138, 135, 134, 129, 127, 126, 129, 128, 125, 130, 127, 123,
        122, 116, 118, 116, 114, 122, 112, 120, 113, 110, 113, 108, 116, 112, 109, 109, 109, 109, 108, 106, 107, 107,
        107, 106, 106, 106, 109, 109, 109, 109, 109, 109, 107, 109, 107, 109, 107, 109, 109, 109, 109, 108, 106, 109,
        108, 106, 109, 108, 106, 109, 108, 106, 109, 108, 106, 109, 108, 106, 109, 108, 106, 109, 108, 106, 109, 108,
        106, 109, 108, 106, 109, 108, 106, 109, 108, 106, 109, 108, 106, 109, 108, 106, 109, 108, 106, 109, 109, 107,
        109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107,
        109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 107, 109, 109, 109, 109,
        106, 109, 108, 106, 109, 106, 109, 106, 109, 108, 106, 111, 109, 108, 106, 112, 111, 106, 113, 112, 111, 110,
        109, 106, 109, 106, 116, 118, 120, 126, 121, 122, 123, 116, 115, 121, 116, 113, 119, 115, 112, 116, 116, 110,
        112, 109, 108, 107, 104, 103, 101,};
uint16_t dutyCycle = 0;
uint16_t pwmPeriod = 10000;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  SH1106_Init();
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start(&htim3);
  
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  
  // SH1106_SetCursor(0,0);
  // SH1106_Fill(Black);
  // SH1106_WriteString("3kb",Font_16x26,White);
  // SH1106_UpdateScreen();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // for (uint8_t i = 0; i < 15; i++)
    // {
    //   SH1106_Fill(Black);
    //   SH1106_DrawBitmap(0, 0, nyanCat[i], 128, 64, White);
    //   SH1106_UpdateScreen();
    //   HAL_Delay(i);
    // }

    //heartbeats
    // for (uint16_t i = 0; i < sizeof(val) / sizeof(val[0]); i++)
    // {
    //   dutyCycle = (val[i] * pwmPeriod) / 255;
    //   htim1.Instance->CCR1 = dutyCycle;
    //   htim3.Instance->CCR1 = dutyCycle;
      
    //   HAL_Delay(1);
    // }

    //NyanCat running
    
    for (uint8_t i = 0; i < 15; i++)
    {
      SH1106_Fill(Black);
      SH1106_DrawBitmap(0, 0, nyanCat[i], 128, 64, White);
      SH1106_UpdateScreen();
      HAL_Delay(34);
    }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
