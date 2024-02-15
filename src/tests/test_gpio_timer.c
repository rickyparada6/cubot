#include "assert.h"
#include "gpio.h"
#include "timer.h"

void test_gpio_set_get_function(void) {
    gpio_init();
    
    // Test get pin function (pin2 defaults to input)
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_INPUT );

    // Test set pin to output
    gpio_set_output(GPIO_PIN2);

    // Test get pin function after setting
    assert( gpio_get_function(GPIO_PIN2) == GPIO_FUNC_OUTPUT );
    
    // Add own test cases:
    //
    // Test set alternate function 
    gpio_set_function(GPIO_PIN2, GPIO_FUNC_ALT1);
    assert(gpio_get_function(GPIO_PIN2) == GPIO_FUNC_ALT1);
    
    // Test different FSEL registers
    gpio_set_function(GPIO_PIN13, GPIO_FUNC_ALT4);
    assert(gpio_get_function(GPIO_PIN13) == GPIO_FUNC_ALT4);

    gpio_set_input(GPIO_PIN45);
    assert(gpio_get_function(GPIO_PIN45) == GPIO_FUNC_INPUT);
    
    // Test multiple GPIO pins in one FSEL register
    gpio_set_function(GPIO_PIN18, GPIO_FUNC_ALT3);
    assert(gpio_get_function(GPIO_PIN18) == GPIO_FUNC_ALT3);
    assert(gpio_get_function(GPIO_PIN13) == GPIO_FUNC_ALT4); // should remain unchanged

    gpio_set_function(GPIO_PIN42, GPIO_FUNC_OUTPUT);
    assert(gpio_get_function(GPIO_PIN42) == GPIO_FUNC_OUTPUT);
    assert(gpio_get_function(GPIO_PIN45) == GPIO_FUNC_INPUT); // should remain unchanged    

    // Test improper requests
    gpio_set_function(54, GPIO_FUNC_ALT5);
    assert(gpio_get_function(GPIO_PIN53) == GPIO_FUNC_ALT3); //should be default ALT3 and not affected by previous line.     
   
    assert(gpio_get_function(100) == GPIO_INVALID_REQUEST);
    
}

void test_gpio_read_write(void) {
    gpio_init();
    gpio_set_function(GPIO_PIN20, GPIO_FUNC_OUTPUT);
     
    // Test gpio_write low, then gpio_read
    gpio_write(GPIO_PIN20, 0);
    assert( gpio_read(GPIO_PIN20) ==  0 );

    // Test gpio_write high, then gpio_read
    gpio_write(GPIO_PIN20, 1);
    assert( gpio_read(GPIO_PIN20) ==  1 );
     
    
    // Add own test cases:
    //
    // Test different FSEL registers
    gpio_set_output(GPIO_PIN31);
    gpio_write(GPIO_PIN31, 0);
    assert(gpio_read(GPIO_PIN31) == 0);
    
    // Test multiple GPIO pins in one FSEL register
    gpio_set_output(GPIO_PIN28);
    gpio_write(GPIO_PIN28, 1);
    gpio_set_output(GPIO_PIN21); 
    gpio_write(GPIO_PIN21, 0);
    assert(gpio_read(GPIO_PIN28) == 1);
    assert(gpio_read(GPIO_PIN21) == 0); 
    assert(gpio_read(GPIO_PIN20) == 1); // should remain unchanged

    
    gpio_set_output(GPIO_PIN39);
    gpio_write(GPIO_PIN39, 0);
    gpio_set_output(GPIO_PIN32);
    gpio_write(GPIO_PIN32, 0);
    assert(gpio_read(GPIO_PIN32) == 0);
    assert(gpio_read(GPIO_PIN39) == 0);
    assert(gpio_read(GPIO_PIN31) == 0); // should remain unchanged

    // Test improper requests
    gpio_set_output(54);
    gpio_write(54, 0);
    
    gpio_write(GPIO_PIN39, 2);
    assert(gpio_read(GPIO_PIN39) == 0); // should remain unchanged

    assert(gpio_read(54) == GPIO_INVALID_REQUEST);
    
}

void test_timer(void) {
    timer_init();

    // Test timer tick count incrementing
    unsigned int start = timer_get_ticks();
    for( int i=0; i<10; i++ ) { /* Spin */ }
    unsigned int finish = timer_get_ticks();
    assert( finish > start );

    // Test timer delay
    int usecs = 100;
    start = timer_get_ticks();
    timer_delay_us(usecs);
    finish = timer_get_ticks();
    assert( finish >= start + usecs );

    // Add own test cases:
    //
    // Test timer delay of 0
    int delay = 0;
    start = timer_get_ticks();
    timer_delay_us(delay);
    finish = timer_get_ticks();
    assert (finish >= start + delay);
}

// Uncomment each call below when you have implemented the functions
// and are ready to test them

void main(void) { 
    test_gpio_set_get_function();
    test_gpio_read_write();
    test_timer();
}
