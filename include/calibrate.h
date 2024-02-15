#ifndef CALIBRATE_H
#define CALIBRATE_H

/*
 * calibrate.h
 * --------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This module allows the user to calibrate the motors to the correct alignment.
 */

#include "motors.h"

void calibrate_init(void);

/*
 * calibrate_motor()
 * -----------
 * Calibrates the specified face with fine-tuning.
 *
 * @param color: the color of the face to turn, as enumerated in 'motors.h'
 * @param dir: the direction of the face to turn, 1 for cw, 0 for ccw
 */
void calibrate_motor(unsigned int color, unsigned int dir);

#endif
