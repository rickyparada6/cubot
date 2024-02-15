/*
 * motors.c
 * --------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This program implements the motor module.
 */

#define ETHAN 0

#include "motors.h"
#include "printf.h"

#define max(x, y) ((x > y) ? x : y)
#define min(x, y) ((x > y) ? y : x)

static int DIR_PINS[] = {DIR_PIN_WHITE, DIR_PIN_YELLOW, DIR_PIN_RED, DIR_PIN_ORANGE, DIR_PIN_GREEN, DIR_PIN_BLUE};
static int STEP_PINS[] = {STEP_PIN_WHITE, STEP_PIN_YELLOW, STEP_PIN_RED, STEP_PIN_ORANGE, STEP_PIN_GREEN, STEP_PIN_BLUE};
static int SLEEP_PINS[] = {SLEEP_PIN_WHITE, SLEEP_PIN_YELLOW, SLEEP_PIN_RED, SLEEP_PIN_ORANGE, SLEEP_PIN_GREEN, SLEEP_PIN_BLUE};
#if ETHAN
static int TUNED_STEPS[6][3] = {{188, 381, 190}, {191, 387, 194}, {188, 380, 190}, {189, 386, 189}, {190, 387, 192}, {186, 382, 189}};
#else
static int TUNED_STEPS[6][3] = {{205, 404, 197}, {202, 396, 197}, {194, 398, 194}, {199, 393, 199}, {201, 399, 197}, {197, 397, 198}};
#endif

void motor_init(void) {
    gpio_init();
    timer_init();
    for (int i = 0; i < 6; i++) {
        gpio_set_output(DIR_PINS[i]);
        gpio_set_output(STEP_PINS[i]);
        gpio_set_output(SLEEP_PINS[i]);
        gpio_write(SLEEP_PINS[i], 0);
    }
}

void motor_move(unsigned int color, unsigned int num_turns) {
    
    gpio_write(SLEEP_PINS[color], 1);
    timer_delay_ms(5); // need delay of 1 ms after wake before issuing step command
    gpio_write(DIR_PINS[color], 3 - max(num_turns, 2)); // DIR set to 1 is CW, so for "R" and "R2" it will be CW, but for "R'" it will be CCW
    for (int i = 0; i < TUNED_STEPS[color][num_turns - 1]; i++) {
        gpio_write(STEP_PINS[color], 1);
        timer_delay_us(250); // set speed of motor: smaller delay = faster.
        gpio_write(STEP_PINS[color], 0);
        timer_delay_us(250);
    }
    gpio_write(SLEEP_PINS[color], 0);
    // timer_delay_ms(10); // set delay between moves.
}
