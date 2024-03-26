/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CW_EN_Pin GPIO_PIN_2
#define CW_EN_GPIO_Port GPIOC
#define DRIVE1_Pin GPIO_PIN_1
#define DRIVE1_GPIO_Port GPIOA
#define DRIVE0_Pin GPIO_PIN_2
#define DRIVE0_GPIO_Port GPIOA
#define SHUTZ_Pin GPIO_PIN_3
#define SHUTZ_GPIO_Port GPIOA
#define TR_EN_Pin GPIO_PIN_4
#define TR_EN_GPIO_Port GPIOA
#define CS0_L_Pin GPIO_PIN_4
#define CS0_L_GPIO_Port GPIOC
#define CS1_L_Pin GPIO_PIN_5
#define CS1_L_GPIO_Port GPIOC
#define CS2_L_Pin GPIO_PIN_0
#define CS2_L_GPIO_Port GPIOB
#define CS3_L_Pin GPIO_PIN_1
#define CS3_L_GPIO_Port GPIOB
#define FLASH_CS_Pin GPIO_PIN_12
#define FLASH_CS_GPIO_Port GPIOB
#define SW_TRIG_Pin GPIO_PIN_6
#define SW_TRIG_GPIO_Port GPIOC
#define UART_RTS_Pin GPIO_PIN_12
#define UART_RTS_GPIO_Port GPIOA
#define TRIG_Pin GPIO_PIN_10
#define TRIG_GPIO_Port GPIOC
#define TRIG_EXTI_IRQn EXTI15_10_IRQn
#define TRIG_INH_Pin GPIO_PIN_11
#define TRIG_INH_GPIO_Port GPIOC
#define LEDG_Pin GPIO_PIN_2
#define LEDG_GPIO_Port GPIOD
#define LEDR_Pin GPIO_PIN_6
#define LEDR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim1;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
