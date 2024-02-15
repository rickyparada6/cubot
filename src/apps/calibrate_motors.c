/*
 * calibrate_motors.c
 * ------------------
 * Author: Ethan Li
 *
 * This program calibrates the motors.
 */

#include "calibrate.h"
#include "printf.h"
#include "uart.h"
#include "keyboard.h"
#include "ps2.h"
#include "interrupts.h"

#define ESC_SCANCODE 0x76

void calibrate_cube(void) {
    while (1) {
        char c = keyboard_read_next();
        if (c == ps2_keys[ESC_SCANCODE].ch) break;
        switch (c) {
            case 'U':
                calibrate_motor(WHITE, 1);
                break;
            case 'D':
                calibrate_motor(YELLOW, 1);
                break;
            case 'R':
                calibrate_motor(RED, 1);
                break;
            case 'L':
                calibrate_motor(ORANGE, 1);
                break;
            case 'F':
                calibrate_motor(GREEN, 1);
                break;
            case 'B':
                calibrate_motor(BLUE, 1);
                break;
        }
    }

}
void main(void) {
    uart_init();
    interrupts_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    interrupts_global_enable();
    calibrate_init();
    calibrate_cube();
    uart_putchar(EOT);
}
