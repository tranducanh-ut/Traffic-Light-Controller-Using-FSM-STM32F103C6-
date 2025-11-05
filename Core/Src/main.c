/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "input_reading.h"
#include "input_processing.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* All defines moved to main.h */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
// ---- FSM State Variables ----
extern enum ModeState mode_status; // Now managed by input_processing.c
enum AutoState auto_status = R1_GREEN_R2_RED;

// ---- Traffic Light Durations (in seconds) ----
int time_red = 5;
int time_amber = 2;
int time_green = 3;
extern int temp_duration; // Managed by input_processing.c

// ---- Countdown Timers ----
int timer_road1_countdown = 0;
int timer_road2_countdown = 0;

// ---- Software Timers (1 tick = 10ms from TIM2) ----
int fsm_timer_counter = 0;
int fsm_timer_flag = 0;
int second_timer_counter = 0;
int second_timer_flag = 0;
int blink_timer_counter = 0;
int blink_timer_flag = 0;
int auto_increment_timer_counter = 0;
int auto_increment_timer_flag = 0;

// ---- 7-Segment Display Variables ----
int led_buffer[4] = {0, 0, 0, 0};
int seg_digit_index = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

// Software Timer Functions
void set_second_timer(int duration_sec);
void set_auto_increment_timer(int duration_ms);
void timer_run(void);

// Display Functions
void updateTrafficLight(void);
void display7SEG(int num);
void update7SEG_Multiplexing(void);
void updateLED_Buffer_Normal(void);
void updateLED_Buffer_Modify(void);

// FSM Functions
void fsm_automatic_run(void);
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);

  // Initial state setup
  set_fsm_timer(time_green);
  set_second_timer(1);
  timer_road1_countdown = time_green;
  timer_road2_countdown = time_red;
  updateTrafficLight();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  fsm_for_input_processing();

	  switch(mode_status){
	  case MODE1_NORMAL:
		  fsm_automatic_run();
		  break;
	  case MODE2_MOD_RED:
		  fsm_for_mode2_red_modify();
		  updateLED_Buffer_Modify();
          if(blink_timer_flag == 1){
              set_blink_timer(250); // 2Hz blink
              HAL_GPIO_TogglePin(TRAFFIC_PORT, RED1_Pin | RED2_Pin);
          }
		  break;
	  case MODE3_MOD_AMBER:
		  fsm_for_mode3_amber_modify();
		  updateLED_Buffer_Modify();
          if(blink_timer_flag == 1){
              set_blink_timer(250);
              HAL_GPIO_TogglePin(TRAFFIC_PORT, YEL1_Pin | YEL2_Pin);
          }
		  break;
	  case MODE4_MOD_GREEN:
		  fsm_for_mode4_green_modify();
		  updateLED_Buffer_Modify();
          if(blink_timer_flag == 1){
              set_blink_timer(250);
              HAL_GPIO_TogglePin(TRAFFIC_PORT, GRE1_Pin | GRE2_Pin);
          }
		  break;
	  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 PA9 PA10 PA11
                           PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB3 PB4
                           PB5 PB6 PB7 PB8
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BUT1_Pin BUT2_Pin BUT3_Pin */
  GPIO_InitStruct.Pin = BUT1_Pin|BUT2_Pin|BUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

// ---- Software Timer Implementation ----
void set_fsm_timer(int duration_sec) {
    fsm_timer_counter = duration_sec * 100;
    fsm_timer_flag = 0;
}
void set_second_timer(int duration_sec) {
    second_timer_counter = duration_sec * 100;
    second_timer_flag = 0;
}
void set_blink_timer(int duration_ms) {
    blink_timer_counter = duration_ms / 10;
    blink_timer_flag = 0;
}
void set_auto_increment_timer(int duration_ms) {
    auto_increment_timer_counter = duration_ms / 10;
    auto_increment_timer_flag = 0;
}

void timer_run(){
    if(fsm_timer_counter > 0){
        fsm_timer_counter--;
        if(fsm_timer_counter <= 0) fsm_timer_flag = 1;
    }
    if(second_timer_counter > 0){
        second_timer_counter--;
        if(second_timer_counter <= 0) second_timer_flag = 1;
    }
    if(blink_timer_counter > 0){
        blink_timer_counter--;
        if(blink_timer_counter <= 0) blink_timer_flag = 1;
    }
    if(auto_increment_timer_counter > 0){
        auto_increment_timer_counter--;
        if(auto_increment_timer_counter <= 0) auto_increment_timer_flag = 1;
    }
}

// ---- Display Control Implementation ----
void clearAllTrafficLEDs(void) {
    HAL_GPIO_WritePin(TRAFFIC_PORT, ALL_TRAFFIC_LIGHT_PINS, GPIO_PIN_SET);
}
void updateTrafficLight(void) {
    // This function is only for normal mode
    clearAllTrafficLEDs();
    switch(auto_status) {
        case R1_GREEN_R2_RED:
            HAL_GPIO_WritePin(TRAFFIC_PORT, GRE1_Pin|RED2_Pin, GPIO_PIN_RESET);
            break;
        case R1_AMBER_R2_RED:
            HAL_GPIO_WritePin(TRAFFIC_PORT, YEL1_Pin|RED2_Pin, GPIO_PIN_RESET);
            break;
        case R1_RED_R2_GREEN:
            HAL_GPIO_WritePin(TRAFFIC_PORT, RED1_Pin|GRE2_Pin, GPIO_PIN_RESET);
            break;
        case R1_RED_R2_AMBER:
            HAL_GPIO_WritePin(TRAFFIC_PORT, RED1_Pin|YEL2_Pin, GPIO_PIN_RESET);
            break;
    }
}
void display7SEG(int num) {
    char segment_code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
    // Turn all segments off first
    HAL_GPIO_WritePin(SEG_PORT, ALL_SEGMENT_PINS, GPIO_PIN_SET);
    // Turn on the required segments
    for(int i = 0; i < 7; i++) {
        if((segment_code[num % 10] >> i) & 1) {
            HAL_GPIO_WritePin(SEG_PORT, 1 << i, GPIO_PIN_RESET);
        }
    }
}
void update7SEG_Multiplexing(void) {
    // Turn all enables off
    HAL_GPIO_WritePin(EN_PORT, EN1_Pin|EN2_Pin|EN3_Pin|EN4_Pin, GPIO_PIN_SET);
    // Display the digit for the current index
    display7SEG(led_buffer[seg_digit_index]);
    // Turn on the corresponding enable pin
    switch(seg_digit_index){
        case 0: HAL_GPIO_WritePin(EN_PORT, EN1_Pin, GPIO_PIN_RESET); break;
        case 1: HAL_GPIO_WritePin(EN_PORT, EN2_Pin, GPIO_PIN_RESET); break;
        case 2: HAL_GPIO_WritePin(EN_PORT, EN3_Pin, GPIO_PIN_RESET); break;
        case 3: HAL_GPIO_WritePin(EN_PORT, EN4_Pin, GPIO_PIN_RESET); break;
    }
    // Move to the next digit
    seg_digit_index = (seg_digit_index + 1) % 4;
}
void updateLED_Buffer_Normal(void){
    led_buffer[0] = timer_road1_countdown / 10;
    led_buffer[1] = timer_road1_countdown % 10;
    led_buffer[2] = timer_road2_countdown / 10;
    led_buffer[3] = timer_road2_countdown % 10;
}
void updateLED_Buffer_Modify(void){
    // 2 LED Äáº§u tiÃªn hiá»n thá» CHáº¾ Ä?á» hiá»n táº¡i (vÃ­ dá»¥: 02, 03, 04)
    led_buffer[0] = 0;
    led_buffer[1] = mode_status +1;

    // 2 LED cÃ²n láº¡i hiá»n thá» GIÃ? TRá» thá»?i gian Äang chá»nh (vÃ­ dá»¥: 12, 54)
    led_buffer[2] = temp_duration / 10;
    led_buffer[3] = temp_duration % 10;
}

// ---- FSM for Automatic Traffic Light Cycle (Normal Mode) ----
void fsm_automatic_run(void) {
    // This logic runs ONCE per second to decrement the countdown
    if (second_timer_flag == 1) {
        set_second_timer(1);
        timer_road1_countdown--;
        timer_road2_countdown--;
        if (timer_road1_countdown < 0)
            timer_road1_countdown = 0;
        if (timer_road2_countdown < 0)
            timer_road2_countdown = 0;
   }

    // This logic runs ONCE when the light state changes
    if (fsm_timer_flag == 1) {
        fsm_timer_flag = 0;
        switch (auto_status) {
            case R1_GREEN_R2_RED:
                auto_status = R1_AMBER_R2_RED;
                set_fsm_timer(time_amber);
                // Reload values for the new state
                timer_road1_countdown = time_amber;
                timer_road2_countdown = time_red - time_green;
                break;
            case R1_AMBER_R2_RED:
                auto_status = R1_RED_R2_GREEN;
                set_fsm_timer(time_green);
                timer_road1_countdown = time_red;
                timer_road2_countdown = time_green;
                break;
            case R1_RED_R2_GREEN:
                auto_status = R1_RED_R2_AMBER;
                set_fsm_timer(time_amber);
                timer_road1_countdown = time_red - time_green;
                timer_road2_countdown = time_amber;
                break;
            case R1_RED_R2_AMBER:
                auto_status = R1_GREEN_R2_RED;
                set_fsm_timer(time_green);
                timer_road1_countdown = time_green;
                timer_road2_countdown = time_red;
                break;
       }
       updateTrafficLight(); // Update the physical LEDs
   }
   // Always update the buffer for the 7-segment display
    updateLED_Buffer_Normal();
}

// ---- Timer Interrupt Callback ----
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM2){
        timer_run();
        button_reading();
        // Use a divider to slow down the scan rate but keep it fast enough for the human eye
        static int scan_counter = 0;
        scan_counter++;
        if (scan_counter >= 10) {
           update7SEG_Multiplexing();
           scan_counter = 0;
        }
    }
}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
