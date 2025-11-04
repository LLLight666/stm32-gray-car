/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_sys.h"
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
#define XIN3_Pin GPIO_PIN_2
#define XIN3_GPIO_Port GPIOE
#define XIN4_Pin GPIO_PIN_3
#define XIN4_GPIO_Port GPIOE
#define XIN5_Pin GPIO_PIN_4
#define XIN5_GPIO_Port GPIOE
#define XIN6_Pin GPIO_PIN_5
#define XIN6_GPIO_Port GPIOE
#define XIN7_Pin GPIO_PIN_6
#define XIN7_GPIO_Port GPIOE
#define XIN8_Pin GPIO_PIN_0
#define XIN8_GPIO_Port GPIOF
#define XIN9_Pin GPIO_PIN_1
#define XIN9_GPIO_Port GPIOF
#define XIN10_Pin GPIO_PIN_2
#define XIN10_GPIO_Port GPIOF
#define XIN11_Pin GPIO_PIN_3
#define XIN11_GPIO_Port GPIOF
#define XIN12_Pin GPIO_PIN_4
#define XIN12_GPIO_Port GPIOF
#define XIN13_Pin GPIO_PIN_5
#define XIN13_GPIO_Port GPIOF
#define XIN14_Pin GPIO_PIN_6
#define XIN14_GPIO_Port GPIOF
#define XIN15_Pin GPIO_PIN_7
#define XIN15_GPIO_Port GPIOF
#define XIN16_Pin GPIO_PIN_8
#define XIN16_GPIO_Port GPIOF
#define E1_Pin GPIO_PIN_9
#define E1_GPIO_Port GPIOF
#define E1_EXTI_IRQn EXTI9_5_IRQn
#define LED_Pin GPIO_PIN_10
#define LED_GPIO_Port GPIOF
#define E2_Pin GPIO_PIN_11
#define E2_GPIO_Port GPIOF
#define E2_EXTI_IRQn EXTI15_10_IRQn
#define E3_Pin GPIO_PIN_12
#define E3_GPIO_Port GPIOF
#define E3_EXTI_IRQn EXTI15_10_IRQn
#define hh_scl_Pin GPIO_PIN_4
#define hh_scl_GPIO_Port GPIOB
#define hh_sda_Pin GPIO_PIN_5
#define hh_sda_GPIO_Port GPIOB
#define XIN1_Pin GPIO_PIN_0
#define XIN1_GPIO_Port GPIOE
#define XIN2_Pin GPIO_PIN_1
#define XIN2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
