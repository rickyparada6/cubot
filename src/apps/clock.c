/* 
 * clock.c
 *
 * Author: Ethan Li
 * Assignment 2 Clock Basic
 *
 * This program utilizes the GPIO and timer modules to control a clock which displays the elapsed minutes and seconds since start.
 *
 */

#include "gpio.h"
#include "timer.h"

// Declare global constants
const unsigned int GPIO_SEGMENT_PINS[8] = {GPIO_PIN20, GPIO_PIN21, GPIO_PIN22, GPIO_PIN23, GPIO_PIN24, GPIO_PIN25, GPIO_PIN26, GPIO_PIN27};
const unsigned int GPIO_DIGIT_PINS[4] = {GPIO_PIN10, GPIO_PIN11, GPIO_PIN12, GPIO_PIN13};
const unsigned int GPIO_INPUT_PINS[1] = {GPIO_PIN2};
const char digits[16] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 
                         0b01100110, 0b01101101, 0b01111101, 0b00000111, 
                         0b01111111, 0b01101111, 0b01110111, 0b01111111, 
                         0b00111001, 0b00111111, 0b01111001, 0b01110001};
const char dash = 0b01000000;

/* 
 * setup()
 *
 * Sets the GPIO pins which control the display to be outputs. Sets the button GPIO pins to be inputs.
 */
static void setup() {
    for (int i = 0; i < (sizeof(GPIO_SEGMENT_PINS) / sizeof(GPIO_SEGMENT_PINS[0])); i++) {
        gpio_set_output(GPIO_SEGMENT_PINS[i]);
    }
    for (int i = 0; i < (sizeof(GPIO_DIGIT_PINS) / sizeof(GPIO_DIGIT_PINS[0])); i++) {
        gpio_set_output(GPIO_DIGIT_PINS[i]);
    }
    for (int i = 0; i < (sizeof(GPIO_INPUT_PINS) / sizeof(GPIO_INPUT_PINS[0])); i++) {
        gpio_set_input(GPIO_INPUT_PINS[i]);
    }
}

/*
 * display()
 *
 * Writes to the segment GPIO pins to display the specified digit number.
 *
 * @param digit: the digit number to display 
 */
void display(char digit) {
    int mask = 1;
    for (int i = 0; i < 8; i++) {
        if ((digit & mask) == 0) {
            gpio_write(GPIO_SEGMENT_PINS[i], 0);
        } else {
            gpio_write(GPIO_SEGMENT_PINS[i], 1);
        }
        digit = digit >> 1;
    }
}

/*
 * refresh()
 *
 * A display refresh loop which drives the four digits by successively turning each on for 2500 us.
 *
 * @param time[4]: an array containing the four digits to display
 */
void refresh(char time[4]) {
    for (int i = 0; i < 4; i++) {
        display(time[i]);
        gpio_write(GPIO_DIGIT_PINS[i], 1);
        timer_delay_us(2500);
        gpio_write(GPIO_DIGIT_PINS[i], 0);
    }
}

/*
 * main()
 *
 * Controls the clock. Waits for user input to start, then continuously loops and increments the time.
 *
 * @param void
 */
void main(void)
{
    setup();
    gpio_init(); 
    
    char time[4] = {dash, dash, dash, dash};
    
    while (gpio_read(GPIO_PIN2) == 1) { // wait for button press
        refresh(time);
    }
    unsigned int currentMin = 0;
    unsigned int currentSec = 0;
    unsigned int start;
    while (1) { // will take a long time until current overflows
        time[0] = digits[(currentMin / 10) % 10];
        time[1] = digits[currentMin % 10];
        time[2] = digits[(currentSec / 10) % 10];
        time[3] = digits[currentSec % 10];
        start = timer_get_ticks();   
        while (timer_get_ticks() - start < 1000000) {
            refresh(time);
        }
        currentSec++;
        if (currentSec == 60) {
            currentSec = 0;
            currentMin++;
        }
        if (currentMin == 60) {
            currentMin = 0;
        }
    }
}
