/*
 * calibrate.c
 * --------
 * Author: Ethan Li
 *
 * This program calibrates the motors.
 */

#include "calibrate.h"

static int DIR_PINS[] = {DIR_PIN_WHITE, DIR_PIN_YELLOW, DIR_PIN_RED, DIR_PIN_ORANGE, DIR_PIN_GREEN, DIR_PIN_BLUE};
static int STEP_PINS[] = {STEP_PIN_WHITE, STEP_PIN_YELLOW, STEP_PIN_RED, STEP_PIN_ORANGE, STEP_PIN_GREEN, STEP_PIN_BLUE};
static int SLEEP_PINS[] = {SLEEP_PIN_WHITE, SLEEP_PIN_YELLOW, SLEEP_PIN_RED, SLEEP_PIN_ORANGE, SLEEP_PIN_GREEN, SLEEP_PIN_BLUE};

void calibrate_init(void) {
    motor_init();
}

void calibrate_motor(unsigned int color, unsigned int dir) {
    gpio_write(SLEEP_PINS[color], 1);
    timer_delay_ms(5); // need delay of 1 ms after wake before issuing step command
    gpio_write(DIR_PINS[color], dir); 
    for (int i = 0; i < 20; i++) {
        gpio_write(STEP_PINS[color], 1);
        timer_delay_us(250);
        gpio_write(STEP_PINS[color], 0);
        timer_delay_us(250);
    }
    gpio_write(SLEEP_PINS[color], 0);
    timer_delay_ms(200);
}
