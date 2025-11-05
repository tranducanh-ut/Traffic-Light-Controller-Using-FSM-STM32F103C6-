/*
 * input_processing.c
 *
 * Created on: Oct 11, 2025
 * Author: tonim
 */

#include "input_processing.h"
#include "input_reading.h"
#include "main.h"

// FSM States for the MODE Button (Button 0)
enum ButtonModeState {
    MODE_BTN_RELEASED,
    MODE_BTN_PRESSED,
    MODE_BTN_PRESSED_1S
};

// FSM States for the Modification Button (Button 1)
enum ButtonModifyState {
    MODIFY_BTN_RELEASED,
    MODIFY_BTN_PRESSED,
    MODIFY_BTN_AUTO_INCREMENT
};

// State variables for the FSMs
static enum ButtonModeState mode_button_state = MODE_BTN_RELEASED;
static enum ButtonModifyState modify_button_state = MODIFY_BTN_RELEASED;

// ---- Global variable for mode status ----
enum ModeState mode_status = MODE1_NORMAL;
extern enum AutoState auto_status;
// ---- Traffic Light Durations (from main.c) ----
extern int time_red;
extern int time_amber;
extern int time_green;
int temp_duration = 0;

// ---- Timer flag for auto-increment ----
extern int auto_increment_timer_flag;


// Forward declaration for the shared FSM function
void fsm_for_modify_button(void);


// ---- FSM for switching between modes (handles MODE Button) ----
void fsm_for_input_processing(void){
    switch(mode_button_state){
        case MODE_BTN_RELEASED:
            if(is_button_pressed(0)){
                mode_button_state = MODE_BTN_PRESSED;

                // ACTION: Change to the next mode and set it up
                mode_status = (enum ModeState)((mode_status + 1) % 4);

                // Reset the modification button's FSM state for the new mode
                modify_button_state = MODIFY_BTN_RELEASED;
                set_auto_increment_timer(0);

                clearAllTrafficLEDs();

                switch(mode_status) {
                    case MODE2_MOD_RED:
                        temp_duration = time_red;
                        set_blink_timer(250);
                        break;
                    case MODE3_MOD_AMBER:
                        temp_duration = time_amber;
                        set_blink_timer(250);
                        break;
                    case MODE4_MOD_GREEN:
                        temp_duration = time_green;
                        set_blink_timer(250);
                        break;
                    case MODE1_NORMAL:
                    default:
                        auto_status = R1_GREEN_R2_RED;
                        set_fsm_timer(time_green);
                        set_second_timer(1);
                        timer_road1_countdown = time_green;
                        timer_road2_countdown = time_red;
                        updateTrafficLight();
                        break;
                }
            }
            break;

        case MODE_BTN_PRESSED:
            if(HAL_GPIO_ReadPin(BUTTON_PORT, BUT1_Pin) == GPIO_PIN_SET){
                mode_button_state = MODE_BTN_RELEASED;
            }
            break;

        case MODE_BTN_PRESSED_1S: // khong co
            if(HAL_GPIO_ReadPin(BUTTON_PORT, BUT1_Pin) == GPIO_PIN_SET){
                mode_button_state = MODE_BTN_RELEASED;
            }
            break;
    }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + SHARED Finite State Machine for the modification button (Button 1)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void fsm_for_modify_button(void){
    switch(modify_button_state){
        case MODIFY_BTN_RELEASED:
            if(is_button_pressed(1)){
                modify_button_state = MODIFY_BTN_PRESSED;
                // ACTION: Increase value by one
                temp_duration++;
                if(temp_duration > 99) temp_duration = 1;
            }
            break;

        case MODIFY_BTN_PRESSED:
            if(HAL_GPIO_ReadPin(BUTTON_PORT, BUT2_Pin) == GPIO_PIN_SET){
                modify_button_state = MODIFY_BTN_RELEASED;
            }
            else if(is_button_pressed_1s(1)){
                modify_button_state = MODIFY_BTN_AUTO_INCREMENT;
                set_auto_increment_timer(500);
            }
            break;

        case MODIFY_BTN_AUTO_INCREMENT:
            if(HAL_GPIO_ReadPin(BUTTON_PORT, BUT2_Pin) == GPIO_PIN_SET){
                modify_button_state = MODIFY_BTN_RELEASED;
                set_auto_increment_timer(0);
            }
            else {
                if(auto_increment_timer_flag == 1){
                    temp_duration++;
                    if(temp_duration > 99) temp_duration = 1;
                    set_auto_increment_timer(500);
                }
            }
            break;
    }
}


// ---- FSM for Mode 2: Modify Red Duration ----
void fsm_for_mode2_red_modify(void) {
    // Run the shared FSM for button 1 logic
    fsm_for_modify_button();

    // Handle the SET button (button 2), which is unique to this mode
    if (is_button_pressed(2)) {
        if (temp_duration > time_amber) {
            time_red = temp_duration;
            time_green = time_red - time_amber;
        }
    }
}

// ---- FSM for Mode 3: Modify Amber Duration ----
void fsm_for_mode3_amber_modify(void) {
    // Run the shared FSM for button 1 logic
    fsm_for_modify_button();

    // Handle the SET button (button 2), which is unique to this mode
    if (is_button_pressed(2)) {
        time_amber = temp_duration;
        time_red = time_green + time_amber;
    }
}

// ---- FSM for Mode 4: Modify Green Duration ----
void fsm_for_mode4_green_modify(void) {
    // Run the shared FSM for button 1 logic
    fsm_for_modify_button();

    // Handle the SET button (button 2), which is unique to this mode
    if (is_button_pressed(2)) {
        time_green = temp_duration;
        time_red = time_green + time_amber;
    }
}
