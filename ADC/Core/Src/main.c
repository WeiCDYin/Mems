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
#include "adc.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ads131.h"
#include "usbd_cdc_if.h"
#include "ntc.h"
#include "vofa_transport.h"
#include <string.h>
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
    MX_USART2_UART_Init();
    MX_ADC5_Init();
    MX_SPI3_Init();
    MX_USART3_UART_Init();
    MX_RTC_Init();
    MX_SPI1_Init();
    MX_SPI4_Init();
    MX_TIM1_Init();
    MX_USB_Device_Init();
    /* USER CODE BEGIN 2 */
    ADS131_Init(ADS_INDEX_1);
    ADS131_Init(ADS_INDEX_2);
    NTC_Init(); // PC9 高电平给 NTC 分压网络供电

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    float vofa_channels[16];
    uint32_t ntc_task_tick = 0; // NTC 100ms 周期任务节拍
#if (ADS131_SAMPLE_MODE == ADS131_SAMPLE_SW_TRIGGER)
    uint32_t trigger_tick  = 0; // ADS131 软件触发节拍
    // 软件触发模式:初始化后进入 STANDBY,等待主循环周期触发
    ADS131_EnterStandby(ADS_INDEX_1);
    ADS131_EnterStandby(ADS_INDEX_2);
    trigger_tick = HAL_GetTick();
#endif

    while (1) {
        // 100ms 周期任务:NTC 温度采样(PA8 NTC / PA9 DVDD 双通道 ADC)
        if (HAL_GetTick() - ntc_task_tick >= 100U) {
            ntc_task_tick = HAL_GetTick();
            NTC_PeriodicTask();
        }

#if (ADS131_SAMPLE_MODE == ADS131_SAMPLE_SW_TRIGGER)
        // 软件触发:按 ADS131_SW_TRIGGER_MS 周期 WAKEUP 一次,DRDY 中断负责读取
        if (HAL_GetTick() - trigger_tick >= ADS131_SW_TRIGGER_MS) {
            trigger_tick = HAL_GetTick();
            ADS131_SoftwareTrigger(ADS_INDEX_1);
            ADS131_SoftwareTrigger(ADS_INDEX_2);
        }
#endif

        // SPI complete (flag == 2): data ready to consume（电压已在中断内算好）
        if (g_ads1_ready_flag == 2) {
            if ((g_ads1_data.status & 0xFFU) == 0xFFU) { memcpy(&vofa_channels[0], (const void *)g_ads1_data.ch_voltage, 8 * sizeof(float)); }
            g_ads1_ready_flag = 0;
        }
        if (g_ads2_ready_flag == 2) {
            if ((g_ads2_data.status & 0xFFU) == 0xFFU) { memcpy(&vofa_channels[8], (const void *)g_ads2_data.ch_voltage, 8 * sizeof(float)); }
            g_ads2_ready_flag = 0;

            // 两片数据齐备,拼接 16 通道按 VOFA+ JustFloat 格式发送
            // 发送端口由 vofa_transport.h 的 VOFA_PORT_SELECT 宏选择:
            // VOFA_PORT_USB / VOFA_PORT_UART2 / VOFA_PORT_UART3 / VOFA_PORT_SPI3
            VOFA_SendFrame(vofa_channels, 16);

#if (ADS131_SAMPLE_MODE == ADS131_SAMPLE_SW_TRIGGER)
            // 软件触发模式：一次采样完成，回到 STANDBY 等待下次触发
            ADS131_EnterStandby(ADS_INDEX_1);
            ADS131_EnterStandby(ADS_INDEX_2);
#endif
        }

        HAL_Delay(1);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 *
 *
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

    /** Configure LSE Drive Capability
     */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
    RCC_OscInitStruct.HSI48State     = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = RCC_PLLM_DIV6;
    RCC_OscInitStruct.PLL.PLLN       = 85;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = RCC_PLLQ_DIV4;
    RCC_OscInitStruct.PLL.PLLR       = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) { Error_Handler(); }
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
    while (1) {}
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
