/*
 * cube_solver.c
 * -------------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This program uses a RPi to control a Rubik's Cube using 6 stepper motors. The user can scramble the cube using the keyboard. Then, the program will determine the solution and perform the sequential algorithms to solve the cube.
 */

#include "printf.h"
#include "uart.h"
#include "keyboard.h"
#include "ps2.h"
#include "interrupts.h"
#include "cube.h"
#include "motors.h"
#include "solver.h"
#include "calibrate.h"
#include <stdbool.h>
#include "malloc.h"
#include "strings.h"
#include <stdarg.h>
#include "gl.h"

#define _WIDTH 640
#define _HEIGHT 512
#define ESC_SCANCODE 0x76
#define MAX_SOLVE_LEN 600
#define MAX_SCRAMBLE_LEN 300

static void draw_sticker(int x, int y, char c) {
    switch(c) {
        case 'W':
            gl_draw_rect(x, y, 40, 40, GL_WHITE);
            break;
        case 'Y':
            gl_draw_rect(x, y, 40, 40, GL_YELLOW);
            break;
        case 'R':
            gl_draw_rect(x, y, 40, 40, GL_RED);
            break;
        case 'O':
            gl_draw_rect(x, y, 40, 40, 0xFFFFA500);
            break;
        case 'G':
            gl_draw_rect(x, y, 40, 40, GL_GREEN);
            break;
        case 'B':
            gl_draw_rect(x, y, 40, 40, GL_BLUE);
            break;
    }
}

static void draw_lines_centers(void) {
    // U
    for (int j = 0; j < 3; j++) {
        for (int i = 124; i < 248; i++) {
            gl_draw_pixel(i, j * 41, GL_BLACK);
        }
    }
    // L, F, R, B
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 496; i++) {
            gl_draw_pixel(i, j * 41 + 123, GL_BLACK);
        }
    }
    // D
    for (int j = 0; j < 3; j++) {
        for (int i = 124; i < 248; i++) {
            gl_draw_pixel(i, j * 41 + 287, GL_BLACK);
        }
    }
    draw_sticker(165, 42, 'W');
    draw_sticker(42, 165, 'O');
    draw_sticker(165, 165, 'G');
    draw_sticker(288, 165, 'R');
    draw_sticker(411, 165, 'B');
    draw_sticker(165, 288, 'Y');
}

static int edge_x_coord[] = {165, 206, 165, 124, 42, 83, 42, 1, 165, 206, 165, 124, 288, 329, 288, 247, 411, 452, 411, 370, 165, 206, 165, 124};
static int edge_y_coord[] = {1, 42, 83, 42, 124, 165, 206, 165, 124, 165, 206, 165, 124, 165, 206, 165, 124, 165, 206, 165, 247, 288, 329, 288};
static int corner_x_coord[] = {124, 206, 206, 124, 1, 83, 83, 1, 124, 206, 206, 124, 247, 329, 329, 247, 370, 452, 452, 370, 124, 206, 206, 124};
static int corner_y_coord[] = {1, 1, 83, 83, 124, 124, 206, 206, 124, 124, 206, 206, 124, 124, 206, 206, 124, 124, 206, 206, 247, 247, 329, 329};

static void draw_cube(void) {
    static bool first_draw = true;
    static bool second_draw = true;
    if (first_draw) {
        draw_lines_centers();
        first_draw = false;
    } else if (second_draw) {
        draw_lines_centers();
        second_draw = false;
    }
    for (char i = 'A'; i <= 'X'; i++) {
        draw_sticker(edge_x_coord[i - 65], edge_y_coord[i - 65], get_color_edge_letter(i));
    }
    for (char i = 'A'; i <= 'X'; i++) {
        draw_sticker(corner_x_coord[i - 65], corner_y_coord[i - 65], get_color_corner_letter(i));
    }
    gl_swap_buffer();
}

static char *scramble_cube(void) {
    char *scramble = malloc(MAX_SCRAMBLE_LEN);
    memset(scramble, '\0', MAX_SCRAMBLE_LEN);
    while (strlen(scramble) < MAX_SCRAMBLE_LEN - 3) {
        char c = keyboard_read_next();
        if (c == ps2_keys[ESC_SCANCODE].ch) break;
        switch (c) {
            case 'U':
                u_move(1);
                motor_move(WHITE, 1);
                strlcat(scramble, "U ", MAX_SCRAMBLE_LEN);                
                break;
            case 'D':
                d_move(1);
                motor_move(YELLOW, 1);
                strlcat(scramble, "D ", MAX_SCRAMBLE_LEN);                
                break;
            case 'R':
                r_move(1);
                motor_move(RED, 1);
                strlcat(scramble, "R ", MAX_SCRAMBLE_LEN);                
                break;
            case 'L':
                l_move(1);
                motor_move(ORANGE, 1);
                strlcat(scramble, "L ", MAX_SCRAMBLE_LEN);                
                break;
            case 'F':
                f_move(1);
                motor_move(GREEN, 1);
                strlcat(scramble, "F ", MAX_SCRAMBLE_LEN);                
                break;
            case 'B':
                b_move(1);
                motor_move(BLUE, 1);
                strlcat(scramble, "B ", MAX_SCRAMBLE_LEN);                
                break;
            case 'u':
                u_move(3);
                motor_move(WHITE, 3);
                strlcat(scramble, "U' ", MAX_SCRAMBLE_LEN);                
                break;
            case 'd':
                d_move(3);
                motor_move(YELLOW, 3);
                strlcat(scramble, "D' ", MAX_SCRAMBLE_LEN);                
                break;
            case 'r':
                r_move(3);
                motor_move(RED, 3);
                strlcat(scramble, "R' ", MAX_SCRAMBLE_LEN);                
                break;
            case 'l':
                l_move(3);
                motor_move(ORANGE, 3);
                strlcat(scramble, "L' ", MAX_SCRAMBLE_LEN);                
                break;
            case 'f':
                f_move(3);
                motor_move(GREEN, 3);
                strlcat(scramble, "F' ", MAX_SCRAMBLE_LEN);                
                break;
            case 'b':
                b_move(3);
                motor_move(BLUE, 3);
                strlcat(scramble, "B' ", MAX_SCRAMBLE_LEN);                
                break;
            case '7':
                u_move(2);
                motor_move(WHITE, 2);
                strlcat(scramble, "U2 ", MAX_SCRAMBLE_LEN);                
                break;
            case '8':
                d_move(2);
                motor_move(YELLOW, 2);
                strlcat(scramble, "D2 ", MAX_SCRAMBLE_LEN);                
                break;
            case '9':
                r_move(2);
                motor_move(RED, 2);
                strlcat(scramble, "R2 ", MAX_SCRAMBLE_LEN);                
                break;
            case '0':
                l_move(2);
                motor_move(ORANGE, 2);
                strlcat(scramble, "L2 ", MAX_SCRAMBLE_LEN);                
                break;
            case '-':
                f_move(2);
                motor_move(GREEN, 2);
                strlcat(scramble, "F2 ", MAX_SCRAMBLE_LEN);                
                break;
            case '=':
                b_move(2);
                motor_move(BLUE, 2);
                strlcat(scramble, "B2 ", MAX_SCRAMBLE_LEN);                
                break;
            case '1':
                calibrate_motor(WHITE, 1);
                break;
            case '2':
                calibrate_motor(YELLOW, 1);
                break;
            case '3':
                calibrate_motor(RED, 1);
                break;
            case '4':
                calibrate_motor(ORANGE, 1);
                break;
            case '5':
                calibrate_motor(GREEN, 1);
                break;
            case '6':
                calibrate_motor(BLUE, 1);
                break;
            case '!':
                calibrate_motor(WHITE, 0);
                break;
            case '@':
                calibrate_motor(YELLOW, 0);
                break;
            case '#':
                calibrate_motor(RED, 0);
                break;
            case '$':
                calibrate_motor(ORANGE, 0);
                break;
            case '%':
                calibrate_motor(GREEN, 0);
                break;
            case '^':
                calibrate_motor(BLUE, 0);
                break;
        }
        printf("%c\n", c);
        draw_cube();
        print_cube_state();
    }
    if (strlen(scramble) >= MAX_SCRAMBLE_LEN - 3) {
        printf("Maximum scramble length reached. Press Esc to begin solve.\n");
        char c = keyboard_read_next();
        while (c != ps2_keys[ESC_SCANCODE].ch) c = keyboard_read_next();
    }
    return scramble;
}

static unsigned int perform_solve(char *solve) {
    unsigned int start_time = timer_get_ticks();
    while (*solve) {
        char move = *solve;
        if (move == ' ' || move == 0x27 || move == '2') {
            solve++;
            continue;
        }
        char modifier = *(solve+1);
        switch (move) {
            case 'U':
                switch (modifier) {
                    case ' ':
                        u_move(1);
                        motor_move(WHITE, 1);
                        break;
                    case 0x27:
                        u_move(3);
                        motor_move(WHITE, 3);
                        break;
                    case '2':
                        u_move(2);
                        motor_move(WHITE, 2);
                        break;
                }
                break;
            case 'D':
                switch (modifier) {
                    case ' ':
                        d_move(1);
                        motor_move(YELLOW, 1);
                        break;
                    case 0x27:
                        d_move(3);
                        motor_move(YELLOW, 3);
                        break;
                    case '2':
                        d_move(2);
                        motor_move(YELLOW, 2);
                        break;
                }
                break;
            case 'R':
                switch (modifier) {
                    case ' ':
                        r_move(1);
                        motor_move(RED, 1);
                        break;
                    case 0x27:
                        r_move(3);
                        motor_move(RED, 3);
                        break;
                    case '2':
                        r_move(2);
                        motor_move(RED, 2);
                        break;
                }
                break;
            case 'L':
                switch (modifier) {
                    case ' ':
                        l_move(1);
                        motor_move(ORANGE, 1);
                        break;
                    case 0x27:
                        l_move(3);
                        motor_move(ORANGE, 3);
                        break;
                    case '2':
                        l_move(2);
                        motor_move(ORANGE, 2);
                        break;
                }
                break;
            case 'F':
                switch (modifier) {
                    case ' ':
                        f_move(1);
                        motor_move(GREEN, 1);
                        break;
                    case 0x27:
                        f_move(3);
                        motor_move(GREEN, 3);
                        break;
                    case '2':
                        f_move(2);
                        motor_move(GREEN, 2);
                        break;
                }
                break;
            case 'B':
                switch (modifier) {
                    case ' ':
                        b_move(1);
                        motor_move(BLUE, 1);
                        break;
                    case 0x27:
                        b_move(3);
                        motor_move(BLUE, 3);
                        break;
                    case '2':
                        b_move(2);
                        motor_move(BLUE, 2);
                        break;
                }
                break;
        }
        solve++;
        //draw_cube();
    }
    unsigned int time_elapsed = timer_get_ticks() - start_time;
    return time_elapsed;
}

static void clear_memory(int size, ...) {
    va_list strings;

    va_start(strings, size);
    for (int i = 0; i < size; i++) {
        char *str = va_arg(strings, char *);

        memset(str, '\0', strlen(str));
        free(str);
    }
    va_end(strings);
}

static char *solve_cube(char *edge_trace, char *corner_trace) {
    char *solve = malloc(MAX_SOLVE_LEN);
    memset(solve, '\0', MAX_SOLVE_LEN);
    while (*edge_trace) {
        char first_letter = *edge_trace;
        char second_letter = *(edge_trace+1);
        char *alg;
        if (first_letter > second_letter) {
            alg = parse_comm(edge_comm_list[second_letter - 65][first_letter - 65], true);
        } else {
            alg = parse_comm(edge_comm_list[first_letter - 65][second_letter - 65], false);
        }
        strlcat(solve, alg, MAX_SOLVE_LEN);
        clear_memory(1, alg);
        edge_trace += 2;
    }

    while (*corner_trace && *(corner_trace+1)) {
        char first_letter = *corner_trace;
        char second_letter = *(corner_trace+1);
        char *alg;
        if (first_letter > second_letter) {
            alg = parse_comm(corner_comm_list[second_letter - 65][first_letter - 65], true);
        } else {
            alg = parse_comm(corner_comm_list[first_letter - 65][second_letter - 65], false);
        }
        strlcat(solve, alg, MAX_SOLVE_LEN);
        clear_memory(1, alg);
        corner_trace += 2;
    }
    
    if (*corner_trace) {
        char letter = *corner_trace;
        char *alg;
        char *parsed = parse_alg(parity_alg_list[letter - 65]);
        alg = clear_rotations(parsed);
        strlcat(solve, alg, MAX_SOLVE_LEN);
        clear_memory(2, parsed, alg);
    }
   
    return clear_duplicates(solve);
}

void main(void) {
    uart_init();
    interrupts_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    motor_init();
    interrupts_global_enable();
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);
    
    while(1) {
        cube_init();
        solve_init();
        print_cube_state();
        gl_draw_rect(1, 450, _WIDTH, gl_get_char_height(), GL_BLACK);
        draw_cube();
        printf("Please enter a scramble with a maximum of ~100 moves. Uppercase letters 'U', 'D', 'R', 'L', 'F', 'B' will turn the respective face clockwise 90 degrees. A corresponding lowercase letter will turn the respective face counterclockwise 90 degrees. Press Esc when ready to begin solve.\n");
        char *scramble = scramble_cube();
        printf("Scramble is %s\n", scramble);
        gl_draw_rect(1, 450, _WIDTH, gl_get_char_height(), GL_BLACK);
        gl_draw_string(1, 450, "Solving!", GL_WHITE);
        draw_cube();
        char *corner_trace = trace_corners();
        char *edge_trace = trace_edges();
        char *solve = solve_cube(edge_trace, corner_trace);
        printf("Solution is %s\n", solve);
        unsigned int time = perform_solve(solve);
        char solve_time[50];
        snprintf(solve_time, sizeof(solve_time), "Cube solved in %d.%d%d seconds\n", time / 1000000, (time / 100000) % 10, (time / 10000) % 10);
        gl_draw_rect(1, 450, _WIDTH, gl_get_char_height(), GL_BLACK);
        gl_draw_string(1, 450, solve_time, GL_WHITE);
        draw_cube();
        printf("Cube solved in %d.%d%d seconds\n", time / 1000000, (time / 100000) % 10, (time / 10000) % 10);
        clear_memory(4, scramble, corner_trace, edge_trace, solve);
        timer_delay(3);
    }
    uart_putchar(EOT);
}
