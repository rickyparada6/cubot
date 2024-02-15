#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"


#define _WIDTH 640
#define _HEIGHT 512

#define _NROWS 10
#define _NCOLS 20

void test_fb(void)
{
    printf("Starting test_fb\n");
    timer_delay(2);

    fb_init(700, 700, 4, FB_SINGLEBUFFER);
    printf("Frame buffer pitch is %d\n", fb_get_pitch());

    unsigned char *cptr = fb_get_draw_buffer();
    int nbytes = fb_get_pitch()*fb_get_height();
    memset(cptr, 0x99, nbytes); // fill entire framebuffer with light gray pixels
    timer_delay(2);

    fb_init(4, 4, 4, FB_SINGLEBUFFER);
    printf("4x4x4 singlebuffer fb pitch is %d\n", fb_get_pitch());
    printf("width: %d, height: %d, depth: %d\n", fb_get_width(), fb_get_height(), fb_get_depth());
    unsigned char *ptr = fb_get_draw_buffer();
    printf("singlebuffer fb addr is %p\n", ptr);
    fb_swap_buffer();
    ptr = fb_get_draw_buffer();
    printf("After swap: fb addr is %p (should be unchanged)\n", ptr);

    fb_init(4, 4, 4, FB_DOUBLEBUFFER);
    printf("4x4x4 doublebuffer fb pitch is %d\n", fb_get_pitch());
    printf("width: %d, height: %d, depth: %d\n", fb_get_width(), fb_get_height(), fb_get_depth());
    ptr = fb_get_draw_buffer();
    printf("fb doublebuffer addr is %p\n", ptr);
    fb_swap_buffer();
    ptr = fb_get_draw_buffer();
    printf("After swap: fb addr is %p (should be -height*pitch = 4*64 = 256)\n", ptr);
    fb_swap_buffer();
    ptr = fb_get_draw_buffer();
    printf("After swap again: fb addr is %p (should be original)\n", ptr);
}

void test_gl(void)
{
    printf("Testing gl\n");

    // Double buffer mode, make sure you test single buffer too!
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55));

    // Draw green pixel at an arbitrary spot
    gl_draw_pixel(_WIDTH/3, _HEIGHT/3, GL_GREEN);
    assert(gl_read_pixel(_WIDTH/3, _HEIGHT/3) == GL_GREEN);

    // Draw pixel out of bounds
    gl_draw_pixel(1000, 1000, GL_GREEN);
    assert(gl_read_pixel(1000, 1000) == 0);

    // Blue rectangle in center of screen
    gl_draw_rect(_WIDTH/2 - 50, _HEIGHT/2 - 50, 100, 100, GL_BLUE);

    // Single amber character
    gl_draw_char(60, 10, 'A', GL_AMBER);

    // String
    gl_draw_string(60, 30, "Hello!", GL_AMBER);

    // Single character but completely out of bounds
    gl_draw_char(1000, 1000, 'Z', GL_AMBER);

    // Single character but half out of bounds
    gl_draw_char(635, 10, '8', GL_AMBER);

    // String half completely out of bounds
    gl_draw_string(610, 40, "Testing!", GL_AMBER);

    // Show buffer with drawn contents
    gl_swap_buffer();
    printf("Should display stuff\n");
    timer_delay(3);

    gl_draw_string(60, 50, "Hi!", GL_AMBER);
    gl_swap_buffer();
    printf("More stuff with previous removed\n");
    timer_delay(2);

    gl_draw_string(60, 50, "Hey there!", GL_AMBER);
    gl_swap_buffer();
    printf("Again, replaced stuff\n");
    timer_delay(2);

    // Testing singlebuffer
    printf("Testing singlebuffer, should display live\n");
    gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER);
    gl_clear(GL_BLACK); 
    gl_draw_rect(_WIDTH/2 - 50, _HEIGHT/2 - 50, 100, 100, GL_GREEN);
    timer_delay(2);

    printf("Char height: %d, char width: %d\n", gl_get_char_height(), gl_get_char_width());
}

void test_console(void)
{
    printf("Testing test_console\n");

    console_init(_NROWS, _NCOLS);

    // 1: "HELLO WORLD"
    console_printf("HELLO WORLD\r");
    timer_delay(1);

    // 1: "HAPPY WORLD"
    // 2: "CODING"
    console_printf("HAPPY\nCODING\n");
    timer_delay(1);

    // Clear
    console_printf("\f");

    // 1: "Goodbye"
    console_printf("Goodbye!\n");
    timer_delay(1);
    console_printf("A string so long that it has to wrap around to the next line and blah\r");
    timer_delay(1);
    console_printf("Now replace part of that string with this\n");
    timer_delay(1);
    console_printf("Print many\n");
    for (int i = 0; i < 20; i++) {
        console_printf("many\n");
        timer_delay_ms(100);
    }
    console_printf("things to test vertical scrolling.\n");
    timer_delay(1);

    console_init(_NROWS, _NCOLS);
    console_printf("Did everything disappear?\n");
    timer_delay(1);

    // Test combination 
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55));
    gl_swap_buffer();
    timer_delay(1);
    gl_clear(GL_BLACK);
    gl_swap_buffer();
    timer_delay(1);
}

void main(void)
{
    uart_init();
    timer_init();
    printf("Executing main() in test_gl_console.c\n");

    test_fb();
    test_gl();
    test_console();

    printf("Completed main() in test_gl_console.c\n");
    uart_putchar(EOT);
}
