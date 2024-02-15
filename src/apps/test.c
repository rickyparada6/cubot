#include "gpio.h"
#include "timer.h"

#define DIR_PIN GPIO_PIN20
#define STEP_PIN GPIO_PIN21

static void setup() {
    gpio_set_output(DIR_PIN);
    gpio_set_output(STEP_PIN);
}

void main(void) {
    gpio_init();
    setup();
    timer_delay(1);
    /*
    while(1) {
        gpio_write(DIR_PIN, 1);
        for (int i = 0; i < 200; i++) {
            gpio_write(STEP_PIN, 1);
            timer_delay_us(750);
            gpio_write(STEP_PIN, 0);
            timer_delay_us(750);
        }
        timer_delay(1);
        gpio_write(DIR_PIN, 0);
        for (int i = 0; i < 200; i++) {
            gpio_write(STEP_PIN, 1);
            timer_delay_us(750);
            gpio_write(STEP_PIN, 0);
            timer_delay_us(750);
        }
        timer_delay(1);
    }
    */
    gpio_write(DIR_PIN, 1);
    while(1) {
        gpio_write(STEP_PIN, 1);
        timer_delay_us(750);
        gpio_write(STEP_PIN, 0);
        timer_delay_us(750);
    }

}

