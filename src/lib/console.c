/*
 * console.c
 * ---------
 *
 * Author: Ethan Li
 * Assignment 6 Graphics Library and Console
 *
 * This program implements a console display which is used with the shell.
 */

#include "console.h"
#include <stdarg.h>
#include "gl.h"
#include "malloc.h"
#include "printf.h"

#define LINE_LEN 80

static void process_char(char ch);
static unsigned char *output = NULL;
static unsigned int num_rows = 0;
static unsigned int num_cols = 0;
static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;

void console_init(unsigned int nrows, unsigned int ncols)
{
    gl_init(ncols * gl_get_char_width(), nrows * (gl_get_char_height() * 1.25), GL_DOUBLEBUFFER); // 125% leading for font
    if (output) free(output);
    output = malloc(nrows * ncols);
    console_clear();
    num_rows = nrows;
    num_cols = ncols;
}

void console_clear(void)
{
    unsigned char (*output_array)[num_cols] = (unsigned char (*)[num_cols])output;
    for (int j = 0; j < num_rows; j++) {
        for (int i = 0; i < num_cols; i++) {
            output_array[j][i] = '\0';
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

int console_printf(const char *format, ...)
{
    char buf[LINE_LEN];
    size_t bufsize = LINE_LEN;
    va_list ap;
    va_start(ap, format);
    int num_char = vsnprintf(buf, bufsize, format, ap);
    va_end(ap);
    
    int i = 0;
    while (buf[i]) {
        process_char(buf[i++]);
    }
    
    gl_clear(GL_BLACK);
    unsigned char (*output_array)[num_cols] = (unsigned char (*)[num_cols])output;
    unsigned int char_width = gl_get_char_width();
    unsigned int char_height = gl_get_char_height();
    unsigned int leading_height = char_height * 1.25;
    for (int j = 0; j < num_rows; j++) {
        for (int i = 0; i < num_cols; i++) {
            gl_draw_char(i * char_width, (j * leading_height) + (leading_height - char_height), output_array[j][i], GL_GREEN); // leading space is above char
        }
    }
    gl_swap_buffer();

	return num_char;
}

/*
 * vertical_shift()
 * ---------------
 * Shifts the contents to be displayed up one line.
 */
static void vertical_shift() { 
    unsigned char (*output_array)[num_cols] = (unsigned char (*)[num_cols])output;
    for (int j = 0; j < num_rows - 1; j++) {
        for (int i = 0; i < num_cols; i++) {
            output_array[j][i] = output_array[j+1][i];
        }
    }
    for (int i = 0; i < num_cols; i++) {
        output_array[num_rows - 1][i] = '\0';
    }
}

/*
 * process_char()
 * -------------
 * Reads the current character and updates the output accordingly.
 *
 * @param ch: the current character
 */
static void process_char(char ch)
{
    unsigned char (*output_array)[num_cols] = (unsigned char (*)[num_cols])output;
    switch(ch) {
        case '\b':
            if (cursor_x != 0) cursor_x--;
            else if (cursor_y != 0) {
                cursor_y--;
                cursor_x = num_cols - 1;
            }
            break;
        case '\r':
            cursor_x = 0;
            break;
        case '\n':
            cursor_x = 0;
            cursor_y++;
            if (cursor_y == num_rows) {
                vertical_shift();
                cursor_y = num_rows - 1;
            }
            break;
        case '\f':
            console_clear();
            break;
        default:
            output_array[cursor_y][cursor_x] = ch;
            cursor_x = (cursor_x + 1) % num_cols;
            if (cursor_x == 0) cursor_y++;
            if (cursor_y == num_rows) {
                vertical_shift();
                cursor_y = num_rows - 1;
            }
    }

}
