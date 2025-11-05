/*
 * input_reading.c
 *
 * Created on: Oct 9, 2025
 * Author: tonim
 */

#include "input_reading.h"
#include "main.h"

// Number of debounce buffers for stability
#define DEBOUNCE_BUFFER_COUNT 3

// Static variables are private to this file
static GPIO_PinState buttonBuffer[N0_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffers[N0_OF_BUTTONS][DEBOUNCE_BUFFER_COUNT];
static uint8_t button_flag[N0_OF_BUTTONS];
static uint16_t counterForButtonPress1s[N0_OF_BUTTONS];
static uint8_t flagForButtonPress1s[N0_OF_BUTTONS];

uint16_t button_pins[N0_OF_BUTTONS] = {BUT1_Pin, BUT2_Pin, BUT3_Pin};
GPIO_TypeDef* button_ports[N0_OF_BUTTONS] = {BUTTON_PORT, BUTTON_PORT, BUTTON_PORT};



void button_reading(void){
    for(uint8_t i = 0; i < N0_OF_BUTTONS; i++){
        // Shift debounce buffers
        for(int j = DEBOUNCE_BUFFER_COUNT - 1; j > 0; j--) {
            debounceButtonBuffers[i][j] = debounceButtonBuffers[i][j-1];
        }
        debounceButtonBuffers[i][0] = HAL_GPIO_ReadPin(button_ports[i], button_pins[i]);

        // Check for stable state
        int is_stable = 1;
        for(int j = 0; j < DEBOUNCE_BUFFER_COUNT - 1; j++) {
            if(debounceButtonBuffers[i][j] != debounceButtonBuffers[i][j+1]) {
                is_stable = 0;
                break;
            }
        }

        if(is_stable){
            // If state changed from released to pressed
            if(buttonBuffer[i] != debounceButtonBuffers[i][0]) {
                buttonBuffer[i] = debounceButtonBuffers[i][0];
                if(buttonBuffer[i] == GPIO_PIN_RESET) { // Button is pressed
                    button_flag[i] = 1; // Set flag for single press detection
                    // Start counting for long press
                    counterForButtonPress1s[i] = 100; // 1s timeout (100 * 10ms)
                }
            }
        }

        // Long press handling
        if(buttonBuffer[i] == GPIO_PIN_RESET){
            if(counterForButtonPress1s[i] > 0){
                counterForButtonPress1s[i]--;
                if(counterForButtonPress1s[i] == 0){
                    flagForButtonPress1s[i] = 1;
                }
            }
        } else {
             flagForButtonPress1s[i] = 0;
        }
    }
}

unsigned char is_button_pressed(unsigned char index){
    if(index >= N0_OF_BUTTONS) return 0;
    if(button_flag[index] == 1) {
        button_flag[index] = 0; // Clear flag after checking
        return 1;
    }
    return 0;
}

unsigned char is_button_pressed_1s(unsigned char index){
    if(index >= N0_OF_BUTTONS) return 0;
    // This flag remains set as long as the button is held down for >1s
    return flagForButtonPress1s[index];
}
