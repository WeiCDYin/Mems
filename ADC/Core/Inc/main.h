/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OPA1_SEL_Pin GPIO_PIN_0
#define OPA1_SEL_GPIO_Port GPIOC
#define OPA2_SEL_Pin GPIO_PIN_1
#define OPA2_SEL_GPIO_Port GPIOC
#define OPA3_SEL_Pin GPIO_PIN_2
#define OPA3_SEL_GPIO_Port GPIOC
#define OPA4_SEL_Pin GPIO_PIN_3
#define OPA4_SEL_GPIO_Port GPIOC
#define SPI1_Ready_Pin GPIO_PIN_3
#define SPI1_Ready_GPIO_Port GPIOA
#define SPI1_Ready_EXTI_IRQn EXTI3_IRQn
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SPI4_Ready_Pin GPIO_PIN_10
#define SPI4_Ready_GPIO_Port GPIOE
#define SPI4_Ready_EXTI_IRQn EXTI15_10_IRQn
#define SPI4_CS_Pin GPIO_PIN_11
#define SPI4_CS_GPIO_Port GPIOE
#define SPI1_RST_Pin GPIO_PIN_6
#define SPI1_RST_GPIO_Port GPIOC
#define SPI4_RST_Pin GPIO_PIN_7
#define SPI4_RST_GPIO_Port GPIOC
#define NTC_VDD_Pin GPIO_PIN_9
#define NTC_VDD_GPIO_Port GPIOC
#define ADC_NTC_Pin GPIO_PIN_8
#define ADC_NTC_GPIO_Port GPIOA
#define ADC_AVDD_Pin GPIO_PIN_9
#define ADC_AVDD_GPIO_Port GPIOA
#define SPI3_CS_Pin GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
