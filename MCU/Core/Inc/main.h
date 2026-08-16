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
#define ADC2_IN1_2P_Pin GPIO_PIN_0
#define ADC2_IN1_2P_GPIO_Port GPIOA
#define ADC1_IN2_1P_Pin GPIO_PIN_1
#define ADC1_IN2_1P_GPIO_Port GPIOA
#define ADC1_IN3_5P_Pin GPIO_PIN_2
#define ADC1_IN3_5P_GPIO_Port GPIOA
#define ADC1_IN4_9P_Pin GPIO_PIN_3
#define ADC1_IN4_9P_GPIO_Port GPIOA
#define ADC2_IN3_6P_Pin GPIO_PIN_6
#define ADC2_IN3_6P_GPIO_Port GPIOA
#define ADC2_IN4_10P_Pin GPIO_PIN_7
#define ADC2_IN4_10P_GPIO_Port GPIOA
#define ADC2_IN5_14P_Pin GPIO_PIN_4
#define ADC2_IN5_14P_GPIO_Port GPIOC
#define ADC3_IN1_3P_Pin GPIO_PIN_1
#define ADC3_IN1_3P_GPIO_Port GPIOB
#define ADC3_IN4_15P_Pin GPIO_PIN_7
#define ADC3_IN4_15P_GPIO_Port GPIOE
#define ADC3_IN2_7P_Pin GPIO_PIN_9
#define ADC3_IN2_7P_GPIO_Port GPIOE
#define ADC3_IN3_11P_Pin GPIO_PIN_13
#define ADC3_IN3_11P_GPIO_Port GPIOE
#define ADC4_IN1_4P_Pin GPIO_PIN_14
#define ADC4_IN1_4P_GPIO_Port GPIOE
#define ADC4_IN2_8P_Pin GPIO_PIN_15
#define ADC4_IN2_8P_GPIO_Port GPIOE
#define ADC4_IN3_12P_Pin GPIO_PIN_12
#define ADC4_IN3_12P_GPIO_Port GPIOB
#define ADC1_IN5_13P_Pin GPIO_PIN_14
#define ADC1_IN5_13P_GPIO_Port GPIOB
#define ADC4_IN5_16P_Pin GPIO_PIN_15
#define ADC4_IN5_16P_GPIO_Port GPIOB
#define NTC_VDD_Pin GPIO_PIN_9
#define NTC_VDD_GPIO_Port GPIOC
#define NTC_ADC_Pin GPIO_PIN_8
#define NTC_ADC_GPIO_Port GPIOA
#define SPI3_CS_Pin GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
