#ifndef MOTORS_H
#define MOTORS_H

/*
 * motors.h
 * --------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This module controls the motors for each face of the cube.
 */

#include "gpio.h"
#include "timer.h"

enum {
    WHITE = 0,
    YELLOW,
    RED,
    ORANGE,
    GREEN,
    BLUE
};

enum {
    DIR_PIN_WHITE = GPIO_PIN20,
    STEP_PIN_WHITE = GPIO_PIN21,
    DIR_PIN_YELLOW = GPIO_PIN22,
    STEP_PIN_YELLOW = GPIO_PIN23,
    DIR_PIN_RED = GPIO_PIN24,
    STEP_PIN_RED = GPIO_PIN25,
    DIR_PIN_ORANGE = GPIO_PIN26,
    STEP_PIN_ORANGE = GPIO_PIN27,
    DIR_PIN_GREEN = GPIO_PIN10,
    STEP_PIN_GREEN = GPIO_PIN11,
    DIR_PIN_BLUE = GPIO_PIN12,
    STEP_PIN_BLUE = GPIO_PIN13,
    SLEEP_PIN_WHITE = GPIO_PIN2,
    SLEEP_PIN_YELLOW = GPIO_PIN5,
    SLEEP_PIN_RED = GPIO_PIN6,
    SLEEP_PIN_ORANGE = GPIO_PIN7,
    SLEEP_PIN_GREEN = GPIO_PIN8,
    SLEEP_PIN_BLUE = GPIO_PIN9
};

void motor_init(void);

/*
 * motor_move()
 * -----------
 * Rotates the specified face the specified move.
 *
 * @param color: the color of the face to turn, as enumerated above
 * @param num_turns: the number of turns to make, where:
 * 1 is single clockwise rotation (e.g. "R")
 * 2 is double rotation (e.g. "R2")
 * 3 is single counterclockwise rotation (e.g. "R'")
 */
void motor_move(unsigned int color, unsigned int num_turns);

#endif
