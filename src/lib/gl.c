/*
 * gl.c
 * ----
 *
 * Author: Ethan Li
 * Assignment 6 Graphics Library and Console
 *
 * This program implements a graphics library for the RPi. It overlays the framebuffer module.
 */
#include "gl.h"
#include "font.h"

void gl_init(unsigned int width, unsigned int height, gl_mode_t mode)
{
    fb_init(width, height, 4, mode);    // use 32-bit depth always for graphics library
}

void gl_swap_buffer(void)
{
    fb_swap_buffer();
}

unsigned int gl_get_width(void)
{
    return fb_get_width();
}

unsigned int gl_get_height(void)
{
    return fb_get_height();
}

/*
 * check_outside_bounds()
 * -------------
 * Checks whether a given pixel is outside the bounds of the framebuffer.
 *
 * @param x, y: the coordinates of the pixel
 *
 * @return int: 1 if out of bounds, 0 if inside bounds
 */
static int check_outside_bounds(int x, int y)
{
    return x >= gl_get_width() || y >= gl_get_height() || x < 0 || y < 0;
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    return 0xFF << 24 | r << 16 | g << 8 | b;
}

void gl_clear(color_t c)
{
    gl_draw_rect(0, 0, gl_get_width(), gl_get_height(), c);    
}

void gl_draw_pixel(int x, int y, color_t c)
{
    if (check_outside_bounds(x, y)) return;
    unsigned int per_row = fb_get_pitch() / fb_get_depth();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    im[y][x] = c;
}

color_t gl_read_pixel(int x, int y)
{   
    if (check_outside_bounds(x, y)) return 0;
    unsigned int per_row = fb_get_pitch() / fb_get_depth();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    return im[y][x];
}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    if (check_outside_bounds(x, y)) return;
    if (x + w > gl_get_width()) w = gl_get_width() - x;
    if (y + w > gl_get_height()) h = gl_get_height() - y;
    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            gl_draw_pixel(i, j, c);
        }
    }
}

void gl_draw_char(int x, int y, int ch, color_t c)
{ 
    if (check_outside_bounds(x, y)) return;
    size_t buflen = font_get_size();
    unsigned char buf[buflen];
    if (font_get_char((char)ch, buf, buflen)) {
        unsigned char (*buf_array)[font_get_width()] = (unsigned char(*)[font_get_width()])buf;
        for (int j = 0; j < font_get_height(); j++) {
            for (int i = 0; i < font_get_width(); i++) {
                if (buf_array[j][i]) {
                    gl_draw_pixel(x + i, y + j, c);
                }
            }
        }
    }
}

void gl_draw_string(int x, int y, const char* str, color_t c)
{ 
    while (*str) {
        if (check_outside_bounds(x, y)) return;
        gl_draw_char(x, y, *str++, c);
        x += font_get_width();
    }
}

unsigned int gl_get_char_height(void)
{
    return font_get_height();
}

unsigned int gl_get_char_width(void)
{
    return font_get_width();    
}
