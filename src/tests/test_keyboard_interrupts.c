#include "interrupts.h"
#include "keyboard.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "gpio_interrupts.h"
#include "gpioextra.h"

static int count_interrupts_pin3 = 0;
static int count_interrupts_pin20 = 0;
static int count_interrupts_pin21 = 0;
static bool test_handler_pin3(unsigned int pc) {
    if (gpio_check_and_clear_event(GPIO_PIN3)) {
        count_interrupts_pin3++;
        return true;
    }
    return false;
}
static bool test_handler_pin20(unsigned int pc) {
    if (gpio_check_and_clear_event(GPIO_PIN20)) {
        count_interrupts_pin20++;
        return true;
    }
    return false;
}
static bool test_handler_pin21(unsigned int pc) {
    if (gpio_check_and_clear_event(GPIO_PIN21)) {
        count_interrupts_pin21++;
        return true;
    }
    return false;
}

void test_gpio_handler(void) {
    gpio_set_input(GPIO_PIN3);
    gpio_set_pullup(GPIO_PIN3);
    gpio_enable_event_detection(GPIO_PIN3, GPIO_DETECT_FALLING_EDGE);
    
    gpio_interrupts_init();
    gpio_interrupts_enable();
    gpio_interrupts_register_handler(GPIO_PIN3, test_handler_pin3);
    printf("Type on your PS/2 keyboard. The program will count the number of falling edges. The count should be a multiple of 11. You have 5 seconds.\n");
    timer_delay(5);
    printf("Time is up. Interrupt count: %d\n", count_interrupts_pin3);
    gpio_interrupts_disable();
    printf("Interrupts temporarily disabled. Try interrupting in the next 3 seconds.\n");
    timer_delay(3);
    printf("Haha, you weren't able to stop me there. Notice that the interrupt count is unchanged: %d\n", count_interrupts_pin3);

    gpio_interrupts_enable();
    printf("Interrupts reenabled. Try interrupting in the next 3 seconds.\n");
    timer_delay(3);
    printf("Did the interrupt count change: %d\n", count_interrupts_pin3);
    //note that count_interrupts_pin3 is 1 greater than expected. bc when disabled, the interrupts were pending but only one gets counted for. this is why we need the ring buffer!

    gpio_set_input(GPIO_PIN20);
    gpio_set_pullup(GPIO_PIN20);
    gpio_enable_event_detection(GPIO_PIN20, GPIO_DETECT_FALLING_EDGE);
    gpio_interrupts_register_handler(GPIO_PIN20, test_handler_pin20);
    gpio_set_input(GPIO_PIN21);
    gpio_set_pullup(GPIO_PIN21);
    gpio_enable_event_detection(GPIO_PIN21, GPIO_DETECT_FALLING_EDGE);
    gpio_interrupts_register_handler(GPIO_PIN21, test_handler_pin21);
    printf("Now we will test simultaneous interrupts using the button. There are two interrupt counts, one for each pin. You have 5 seconds. Go!\n");
    timer_delay(5);
    printf("How did it do? 20: %d 21: %d\n", count_interrupts_pin20, count_interrupts_pin21);
}

void test_clock_events(void)
{
    printf("Type on your PS/2 keyboard to generate clock events. You've got 10 seconds, go!\n");
    timer_delay(10);
    printf("Time's up!\n");
}


/*
 * This function tests the behavior of the assign5 keyboard
 * implementation versus the new-improved assign7 version. If using the
 * assign5 implementation, a key press that arrives while the main program
 * is waiting in delay is simply dropped. Once you upgrade your
 * keyboard implementation to be interrupt-driven, those keys should
 * be queued up and can be read after delay finishes.
 */
void test_read_delay(void)
{
    while (1) {
        printf("Test program waiting for you to press a key (q to quit): ");
        uart_flush();
        char ch = keyboard_read_next();
        printf("\nRead: %c\n", ch);
        if (ch == 'q') break;
        printf("Test program will now pause for 1 second... ");
        uart_flush();
        timer_delay(1);
        printf("done.\n");
    }
    printf("\nGoodbye!\n");
}


void main(void)
{
    gpio_init();
    timer_init();
    uart_init();
    interrupts_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    interrupts_global_enable();
    
    //test_gpio_handler();
    test_clock_events();  // wait 10 seconds for clock_edge handler to report clock edges
    test_read_delay();  // what happens to keys typed while main program blocked in delay?
    uart_putchar(EOT);
}
