/* 
 * gpio.c
 *
 * Author: Ethan Li
 * Assignment 2 Clock
 *
 * This program implements the module which controls the GPIO peripheral pins of the Raspberry Pi.
 *
 * Function descriptions are in the gpio.h file.
 *
 */

#include "gpio.h"

struct gpio {
    unsigned int FSEL[6];
    unsigned int resA;
    unsigned int SET[2];
    unsigned int resB;
    unsigned int CLR[2];
    unsigned int resC;
    unsigned int LEV[2];
};

volatile struct gpio *gpio = (struct gpio *)0x20200000;

void gpio_init(void) {
}

void gpio_set_function(unsigned int pin, unsigned int function) {
    if (pin > GPIO_PIN_LAST || function > 7) {
        return;
    }
    unsigned int clear = 7; // bin 111
    unsigned int mask = function;
    clear = clear << (3 * (pin % 10));
    mask = mask << (3 * (pin % 10));
    clear = ~clear;
    gpio->FSEL[pin / 10] = gpio->FSEL[pin / 10] & clear;
    gpio->FSEL[pin / 10] = gpio->FSEL[pin / 10] | mask;
}
 
unsigned int gpio_get_function(unsigned int pin) {
    if (pin > GPIO_PIN_LAST) {
        return GPIO_INVALID_REQUEST;
    }  
    unsigned int clear = 7; // bin 111
    unsigned int function;
    clear = clear << (3 * (pin % 10));
    function = gpio->FSEL[pin / 10] & clear;
    function = function >> (3 * (pin % 10));
    return function;
}

void gpio_set_input(unsigned int pin) {
    if (pin > GPIO_PIN_LAST) {
        return;
    }
    gpio_set_function(pin, GPIO_FUNC_INPUT);
}

void gpio_set_output(unsigned int pin) { 
    if (pin > GPIO_PIN_LAST) {
        return;
    }
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

void gpio_write(unsigned int pin, unsigned int value) {
    if (pin > GPIO_PIN_LAST || value > 1) {
        return;
    }
    unsigned int mask = 1;
    if (value == 1) {
        mask = mask << (pin % 32);
        gpio->SET[pin / 32] = mask;
    } else { //value = 0
        mask = mask << (pin % 32);
        gpio->CLR[pin / 32] = mask;
    }
}

unsigned int gpio_read(unsigned int pin) { 
    if (pin > GPIO_PIN_LAST) {
        return GPIO_INVALID_REQUEST;
    }
    unsigned int value;
    unsigned int mask = 1 << (pin % 32);
    if ((gpio->LEV[pin / 32] & mask) == 0) {
        value = 0;
    } else {
        value = 1;
    }
    return value;
}
