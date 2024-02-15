/*
 * keyboard.c
 *
 * Author: Ethan Li
 * Assignment 5 Keyboard and Simple Shell
 *
 * This program implements a keyboard module that interfaces with a PS/2 keyboard.
 */

#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"
#include "timer.h"
#include "gpio_interrupts.h"
#include "ringbuffer.h"

#define TIMEOUT_RESET 3000
#define L_SHIFT_SCANCODE 0x12
#define R_SHIFT_SCANCODE 0x59
#define ALT_SCANCODE 0x11
#define CTRL_SCANCODE 0x14
#define CAPS_SCANCODE 0x58
#define NUM_SCANCODE 0x77
#define SCROLL_SCANCODE 0x7E

static unsigned int CLK, DATA;
static keyboard_modifiers_t mod = 0;
static rb_t *rb;

// Global variables for reading scancode
static unsigned int bit_count = 0;
static unsigned int parity_count = 0;
static unsigned int time = 0;
static unsigned char data = 0;

/*
 * reset_global_vars()
 * -------------------
 * Resets the global variables for reading scancode to 0.
 */
static void reset_global_vars(void) {
    bit_count = 0;
    parity_count = 0;
    time = 0;
    data = 0;
}

/*
 * CLK_handler()
 * ------------
 * The handler for the CLK line. Called when CLK detects a falling edge. Reads the 11 bits of a scancode and verifies the bits.
 *
 * @param pc: the value of the interrupted pc
 *
 * @return bool: whether the interrupt was handled successfully
 */
static bool CLK_handler(unsigned int pc) {
    if (gpio_check_and_clear_event(CLK)) {
        if (time == 0) time = timer_get_ticks(); // for first clock edge of scancode
        if (timer_get_ticks() - time > TIMEOUT_RESET) {
            reset_global_vars();
            return true;
        }
        unsigned int bit = gpio_read(DATA);
        if (bit_count == 0 && bit) { // verify start bit
            reset_global_vars();         
        } else if (bit_count == 0) {
            bit_count++;
            time = timer_get_ticks();
        }
        else if (bit_count == 10) { // verify stop bit
            if (bit) rb_enqueue(rb, (int)data);
            reset_global_vars();
        } else {
            if (bit == 1) parity_count++;
            if (bit_count == 9) { 
                if (!(parity_count % 2)) { // verify parity
                    reset_global_vars();
                    return true;
                }
            } else {
                data |= bit << (bit_count - 1);
            }
            bit_count++;
            time = timer_get_ticks();
        }
        return true;
    }
    return false;
}

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio) 
{
    CLK = clock_gpio;
    gpio_set_input(CLK);
    gpio_set_pullup(CLK);
    gpio_enable_event_detection(CLK, GPIO_DETECT_FALLING_EDGE);
 
    DATA = data_gpio;
    gpio_set_input(DATA);
    gpio_set_pullup(DATA);
    
    gpio_interrupts_init();
    gpio_interrupts_enable();
    gpio_interrupts_register_handler(CLK, CLK_handler);

    rb = rb_new();
}

/*
 * update_modifiers()
 * -----------------
 * Updates the current modifiers in effect such that Shift, Alt, and Ctrl
 * are active iff the corresponding modifier key is depressed, while 
 * Caps Lock, Num Lock, and Scroll Lock are sticky.
 *
 * @param action: the current event action.
 */
static void update_modifiers(key_action_t action) {
    switch(action.keycode) {
        case L_SHIFT_SCANCODE:
            mod = (action.what == KEY_PRESS) ? mod | KEYBOARD_MOD_SHIFT : (mod & ~KEYBOARD_MOD_SHIFT);
            break;
        case R_SHIFT_SCANCODE:
            mod = (action.what == KEY_PRESS) ? mod | KEYBOARD_MOD_SHIFT : (mod & ~KEYBOARD_MOD_SHIFT);
            break;
        case ALT_SCANCODE:
            mod = (action.what == KEY_PRESS) ? mod | KEYBOARD_MOD_ALT : (mod & ~KEYBOARD_MOD_ALT);
            break;
        case CTRL_SCANCODE:
            mod = (action.what == KEY_PRESS) ? mod | KEYBOARD_MOD_CTRL : (mod & ~KEYBOARD_MOD_CTRL);
            break;
        case CAPS_SCANCODE:
            if (action.what == KEY_PRESS) {
                mod = ((mod & KEYBOARD_MOD_CAPS_LOCK) == 0) ? mod | KEYBOARD_MOD_CAPS_LOCK : (mod & ~KEYBOARD_MOD_CAPS_LOCK);
            }
            break;
        case SCROLL_SCANCODE:
            if (action.what == KEY_PRESS) {
                mod = ((mod & KEYBOARD_MOD_SCROLL_LOCK) == 0) ? mod | KEYBOARD_MOD_SCROLL_LOCK : (mod & ~KEYBOARD_MOD_SCROLL_LOCK);
            }
            break;
        case NUM_SCANCODE:
            if (action.what == KEY_PRESS) {
                mod = ((mod & KEYBOARD_MOD_NUM_LOCK) == 0) ? mod | KEYBOARD_MOD_NUM_LOCK : (mod & ~KEYBOARD_MOD_NUM_LOCK);
            }
            break;
    }
}

/*
 * is_modifier()
 * ------------
 * Helper function which returns whether a given keycode is a modifier keycode.
 *
 * @param keycode: the keycode of the current event action.
 * @return int: whether the given keycode is a modifier keycode.
 */
static int is_modifier(unsigned char keycode) {
    return (keycode == L_SHIFT_SCANCODE) || (keycode == R_SHIFT_SCANCODE) || (keycode == ALT_SCANCODE) || (keycode == CTRL_SCANCODE) || (keycode == CAPS_SCANCODE) || (keycode == SCROLL_SCANCODE) || (keycode == NUM_SCANCODE);
}

unsigned char keyboard_read_scancode(void) {
    while (rb_empty(rb));
    int *scancode = 0;
    rb_dequeue(rb, scancode);
    return *(char *)scancode;
}

key_action_t keyboard_read_sequence(void) {
    key_action_t action;
    action.keycode = keyboard_read_scancode();
    if (action.keycode == 0xE0) {
        action.keycode = keyboard_read_scancode();
    }
    if (action.keycode == 0xF0) {
        action.keycode = keyboard_read_scancode();
        action.what = KEY_RELEASE;
    } else {
        action.what = KEY_PRESS;
    }
    return action;
}

key_event_t keyboard_read_event(void) {
    key_event_t event;
    event.action = keyboard_read_sequence();
    update_modifiers(event.action);
    event.key = ps2_keys[event.action.keycode];
    event.modifiers = mod;
    return event;
}

unsigned char keyboard_read_next(void) {
    key_event_t event = keyboard_read_event();
    while (is_modifier(event.action.keycode) || event.action.what == KEY_RELEASE) event = keyboard_read_event();
    char c = event.key.ch;
    keyboard_modifiers_t cur_mod = event.modifiers;
    if (c < '\t' || c > 0x7F) {
        return c;
    } else if (cur_mod & KEYBOARD_MOD_SHIFT) {
        if (event.key.other_ch) c = event.key.other_ch;
    } else if (cur_mod & KEYBOARD_MOD_CAPS_LOCK) {
        if (c >= 'a' && c <= 'z') c = event.key.other_ch;
    }
    return c;
}
