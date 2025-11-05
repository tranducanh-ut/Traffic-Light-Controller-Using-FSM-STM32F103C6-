/* Tệp: main.h */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  * This file contains the common defines of the application.
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

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
// FSM Level 1: Mode Management States
enum ModeState {MODE1_NORMAL = 0, MODE2_MOD_RED, MODE3_MOD_AMBER, MODE4_MOD_GREEN};

// FSM Level 2: Automatic Traffic Light Cycle States
enum AutoState {R1_GREEN_R2_RED, R1_AMBER_R2_RED, R1_RED_R2_GREEN, R1_RED_R2_AMBER};
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
// Make these functions available globally
void set_fsm_timer(int duration_sec);
void set_blink_timer(int duration_ms);
void set_second_timer(int duration_ms);
void clearAllTrafficLEDs(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
// ---- Traffic Light LEDs (Port A) ----
#define RED1_Pin GPIO_PIN_0
#define YEL1_Pin GPIO_PIN_1
#define GRE1_Pin GPIO_PIN_2
#define RED2_Pin GPIO_PIN_3
#define YEL2_Pin GPIO_PIN_4
#define GRE2_Pin GPIO_PIN_5
#define TRAFFIC_PORT GPIOA
#define ALL_TRAFFIC_LIGHT_PINS (RED1_Pin|YEL1_Pin|GRE1_Pin|RED2_Pin|YEL2_Pin|GRE2_Pin)

// ---- 7-Segment Enables (Port A) ----
#define EN1_Pin GPIO_PIN_8
#define EN2_Pin GPIO_PIN_9
#define EN3_Pin GPIO_PIN_10
#define EN4_Pin GPIO_PIN_11
#define EN_PORT GPIOA

// ---- 7-Segment Segments (Port B) ----
#define SEG0_Pin GPIO_PIN_0
#define SEG1_Pin GPIO_PIN_1
#define SEG2_Pin GPIO_PIN_2
#define SEG3_Pin GPIO_PIN_3
#define SEG4_Pin GPIO_PIN_4
#define SEG5_Pin GPIO_PIN_5
#define SEG6_Pin GPIO_PIN_6
#define SEG_PORT GPIOB
#define ALL_SEGMENT_PINS (SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin|SEG4_Pin|SEG5_Pin|SEG6_Pin)

// ---- Buttons (Port B) ----
#define BUT1_Pin GPIO_PIN_13 // Mode Button
#define BUT2_Pin GPIO_PIN_14 // Modify/Increase Button
#define BUT3_Pin GPIO_PIN_15 // Set Button
#define BUTTON_PORT GPIOB

// ---- General Constants ----
#define N0_OF_BUTTONS 3

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
