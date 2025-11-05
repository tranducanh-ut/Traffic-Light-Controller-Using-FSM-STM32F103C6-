/*
 * input_processing.h
 *
 *  Created on: Oct 9, 2025
 *      Author: tonim
 */

#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#include "main.h"


// Biến toàn cục để main.c có thể truy cập
extern enum ModeState mode_status;

// Khai báo các biến thời gian và biến tạm từ main.c
extern int time_red;
extern int time_amber;
extern int time_green;
extern int temp_duration;
extern int timer_road1_countdown;
extern int timer_road2_countdown;
extern void updateTrafficLight(void);
// Khai báo các hàm cần thiết từ main.c
extern void set_fsm_timer(int duration_sec);
extern void clearAllLEDs();
extern void blinkAllRedLEDs(int state);
extern void blinkAllAmberLEDs(int state);
extern void blinkAllGreenLEDs(int state);
extern void displayModeAndValue(enum ModeState mode, int value);
extern void set_blink_timer(int duration_ms); // Khai báo thêm hàm set_blink_timer

// Hàm FSM cho việc xử lý input và chuyển đổi chế độ
void fsm_for_input_processing(void);

// Các hàm cho chế độ chỉnh sửa
void fsm_for_mode2_red_modify(void);
void fsm_for_mode3_amber_modify(void);
void fsm_for_mode4_green_modify(void);

extern void set_auto_increment_timer(int duration_ms);
#endif /* INC_INPUT_PROCESSING_H_ */
