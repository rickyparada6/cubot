#include "assert.h"
#include "keyboard.h"
#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "shell.h"

#define ESC_SCANCODE 0x76

static void test_keyboard_scancodes(void) 
{
    printf("\nNow reading single scancodes. Type ESC to finish this test.\n");
    while (1) {
        unsigned char scancode = keyboard_read_scancode();
        printf("[%02x]", scancode);
        if (scancode == ESC_SCANCODE) break;
    }
    printf("\nDone with scancode test.\n");
}

static void test_keyboard_sequences(void)
{
    printf("\nNow reading scancode sequences (key actions). Type ESC to finish this test.\n");
    while (1) {
        key_action_t action = keyboard_read_sequence();
        printf("%s [%02x]\n", action.what == KEY_PRESS ? "  Press" :"Release", action.keycode);
        if (action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with scancode sequences test.\n");
}

static void test_keyboard_events(void) 
{
    printf("\nNow reading key events. Type ESC to finish this test.\n");
    while (1) {
        key_event_t evt = keyboard_read_event();
        printf("%s PS2_key: {%c,%c} Modifiers: 0x%x\n", evt.action.what == KEY_PRESS? "  Press" : "Release", evt.key.ch, evt.key.other_ch, evt.modifiers);
        if (evt.action.keycode == ESC_SCANCODE) break;
    }
    printf("Done with key events test.\n");
}

static void test_keyboard_chars(void) 
{
    printf("\nNow reading chars. Type ESC to finish this test.\n");
    while (1) {
        char c = keyboard_read_next();
        if (c >= '\t' && c <= 0x80) 
            printf("%c", c);
        else
            printf("[%02x]", c);
        if (c == ps2_keys[ESC_SCANCODE].ch) break;
    }
    printf("\nDone with key chars test.\n");
}

static void test_keyboard_assert(void)
{
    char ch;
    printf("\nHold down Shift and type 'g'\n");
    ch = keyboard_read_next();
    assert(ch == 'G');  // confirm user can follow directions and correct key char generated
    printf("\nPress Caps Lock once and type 'h'\n");
    ch = keyboard_read_next();
    assert(ch == 'H');
    printf("\nNow press '1'\n");
    ch = keyboard_read_next();
    assert(ch == '1');
    printf("\nNow hold down Shift and type '1'\n");
    ch = keyboard_read_next();
    assert(ch == '!');
}

static void test_shell_eval(void) {
    char *cmds[] = {
        "help",
        "help  ",
        "  help   ",
        "help help",
        "help peek poke", // should only display help for peek
        "echo hi",
        "echo    ",
        "echo help",
        "echo hello this is a not so long string",
        "peek", // fails arguments
        "peek 0",
        "peek 3", // fails, not aligned to 4-byte
        "peek 0x800c",
        "peek 0x80hd", // fails, cannot convert
        "peek 0x8000 hi", // ignores 'hi'
        "poke", // fails arguments
        "poke hi", // fails arguments
        "poke 0x20200000", // fails arguments
        "poke 0x202000hd", // fails arguments
        "poke 0x20200000 hi", // fails convert
        "poke 0x20200008 1", // GPIO20 configured output, no printed output
        "peek 0x20200008", // should be 1
        "poke 0x2020001c 0x100000", // GPIO20 set ON, if you attach LED it should light up
        "poke 0x20200028 0x100000" // GPIO20 clr OFF
        // "reboot" do this manually
    };
    for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        shell_evaluate(cmds[i]);
        timer_delay_ms(250);
    }
}

void main(void) 
{
    uart_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    printf("Testing keyboard module.\n");

    test_keyboard_scancodes();
    timer_delay_ms(500);

    test_keyboard_sequences();
    timer_delay_ms(500);

    test_keyboard_events();
    timer_delay_ms(500);

    test_keyboard_chars();

    test_keyboard_assert();

    shell_init(printf);
    test_shell_eval();

    printf("All done!\n");
    uart_putchar(EOT);
}
