#include <stdbool.h>
#include "gpio_interrupts.h"
#include "assert.h"
#include "gpioextra.h"

/*
 * Module to configure GPIO interrupts for Raspberry Pi.
 * Because all of the GPIO pins share a small set of GPIO
 * interrupts, you need a level of indirectiom to be able
 * to handle interrupts for particular pins. This module
 * allows a client to register one handler for each
 * GPIO pin.
 *
 * Author: Philip Levis <pal@cs.stanford.edu>
 * Author: Ethan Li
 * Assignment 7 System Monitor with Interrupts
 *
 * Last update:   May 2020
 */

#define GPIO_PIN_COUNT 54

// Struct array to store handlers for each GPIO pin.
static struct {
    handler_fn_t fn;
} handlers[GPIO_PIN_COUNT];

bool gpio_default_handler(unsigned int pc) { return false; }

// A bitset to mark which pins have non-default handlers registered. Used to optimize the below handler.
static unsigned long long pin_handlers_bitset = 0;

/*
 * gpio_handler()
 * -------------
 * A handler for the general GPIO interrupt, which checks which of the registered pins have events detected and calls the corresponding handler.
 *
 * @param pc: value of interrupted pc
 * 
 * @return bool: whether the interrupt was successfully processed
 */
static bool gpio_handler(unsigned int pc) {
    bool success = false;
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        if ((pin_handlers_bitset & (1 << i)) > 0) {
            if (gpio_check_event(i)) {
                handlers[i].fn(pc);
                success = true;
            }
        }
    }
    return success;
}

/*
 * `gpio_interrupts_init`
 *
 * Initialize the GPIO interrupt modules. The init function must be 
 * called before any calls to other functions in this module.
 * The init function configures gpio interrupts to a clean state.
 * This function registers a handler for GPIO interrupts, which it
 * uses to call handlers on a per-pin basis.
 *
 * Disables GPIO interrupts if they were previously enabled.
 * 
 */
void gpio_interrupts_init(void) {
    gpio_interrupts_disable();
    for (int i = 0; i < GPIO_PIN_COUNT; i++) {
        gpio_interrupts_register_handler(i, gpio_default_handler);
    }
    pin_handlers_bitset = 0;
    interrupts_register_handler(INTERRUPTS_GPIO3, gpio_handler);
}

/*
 * `gpio_interrupts_enable`
 *
 * Enables GPIO interrupts.
 */
void gpio_interrupts_enable(void) {
    interrupts_enable_source(INTERRUPTS_GPIO3);
}

/*
 * `gpio_interrupts_disable`
 *
 * Disables GPIO interrupts.
 */
void gpio_interrupts_disable(void) {
    interrupts_disable_source(INTERRUPTS_GPIO3);
}

/* 
 * `interrupts_attach_handler`
 *
 * Attach a handler function to a given GPIO pin. Each pin
 * source can have one handler: further dispatch should be invoked by
 * the handler itself. Whether or not a particular pin will
 * generate interrupts is specified by the events system,
 * defined in `gpioextra.h`. 
 *
 * Asserts if failed to install handler (e.g., the pin is invalid).
 * Pins are defined in `gpio.h`.
 */
handler_fn_t gpio_interrupts_register_handler(unsigned int pin, handler_fn_t fn) {
    assert(pin < GPIO_PIN_COUNT);
    pin_handlers_bitset |= 1 << pin;
    handler_fn_t old_handler = handlers[pin].fn;
    handlers[pin].fn = fn;
    return old_handler;
}

