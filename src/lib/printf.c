/*
 * prinf.c
 *
 * Author: Ethan Li
 * Assignment 3 Implement a String Formatting Library
 *
 * This program implements a pared-down version of the printf module.
 *
 * Function descriptions are in the printf.h file.
 */

#include "printf.h"
#include "printf_internal.h"
#include "strings.h"
#include "uart.h"
#include <stdarg.h>

#define MAX_OUTPUT_LEN 1024

int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, int min_width)
{
    int n = 0;
    int numDigits = 1;
    unsigned int tempval = val;
    while (tempval / base > 0) {
        tempval = tempval / base;
        n++;
        numDigits++;
    }
    if (bufsize > 0) {
        while (n < min_width - 1) {
            n++;
        }
        tempval = val;
        while (n > ((signed int)bufsize) - 2) {
            n--;
            tempval = tempval / base;
        }
        buf[n + 1] = '\0';
        while (n >= 0) {
            buf[n] = (tempval % base) + 48;
            if (buf[n] > 57) {
                buf[n] += 39;
            }
            tempval = tempval / base;
            n--;
        }
    }
    if (numDigits > min_width) {
        return numDigits;
    } else {
        return min_width;
    }
}

int signed_to_base(char *buf, size_t bufsize, int val, int base, int min_width)
{
    if (val < 0 && bufsize > 1) {
        buf[0] = 45;
        return 1 + unsigned_to_base(buf + 1, bufsize - 1, val * -1, base, min_width - 1);
    }
    return unsigned_to_base(buf, bufsize, val, base, min_width);
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
    int ntemp = 0;
    int nformat = 0;

    char tempbuf[MAX_OUTPUT_LEN];
    size_t tempbufsize = MAX_OUTPUT_LEN;
    memset(tempbuf, 0x77, sizeof(tempbuf)); 

    while (format[nformat] != '\0') {
        if (format[nformat] == '%') {
            nformat++;
            if (format[nformat] == '%') {
                tempbuf[ntemp] = '%';
                ntemp++;
                nformat++;
            } else if (format[nformat] == 'c') {
                tempbuf[ntemp] = (char)va_arg(args, int);
                ntemp++;
                nformat++;
            } else if (format[nformat] == 's') {
                tempbuf[ntemp] = '\0'; // so that strlcat starts cat at current position
                ntemp = strlcat(tempbuf, (char *)va_arg(args, int), tempbufsize);
                nformat++;
            } else if (format[nformat] == 'd') {
                ntemp += signed_to_base(tempbuf + ntemp, tempbufsize - ntemp, va_arg(args, int), 10, 0);
                nformat++;
            } else if (format[nformat] == 'x') {
                ntemp += unsigned_to_base(tempbuf + ntemp, tempbufsize - ntemp, va_arg(args, unsigned int), 16, 0);
                nformat++;
            } else if (format[nformat] == '0') {
                const char *letter = NULL;
                unsigned int width = strtonum(&format[nformat + 1], &letter);
                if (*letter == 'd') {
                    ntemp += signed_to_base(tempbuf + ntemp, tempbufsize - ntemp, va_arg(args, int), 10, width);
                    nformat = nformat + ((&*letter - &format[nformat]) / sizeof(char)) + 1;
                } else { 
                    ntemp += unsigned_to_base(tempbuf + ntemp, tempbufsize - ntemp, va_arg(args, unsigned int), 16, width);
                    nformat = nformat + ((&*letter - &format[nformat]) / sizeof(char)) + 1;
                }
            } else if (format[nformat] == 'p') {
                tempbuf[ntemp] = '0';
                tempbuf[ntemp + 1] = 'x';
                ntemp += 2;
                ntemp += unsigned_to_base(tempbuf + ntemp, tempbufsize - ntemp, va_arg(args, unsigned int), 16, 8);
                nformat++;
            } else { // will just print the % and move on
                tempbuf[ntemp] = format[nformat - 1];
                ntemp++;
            }
        } else {
            tempbuf[ntemp] = format[nformat];
            ntemp++;
            nformat++;
        }
    }
    tempbuf[ntemp] = '\0';

    if (bufsize > 0) {
        //copy tempbuf into buf
        if (ntemp < bufsize - 1) {
            memcpy(buf, tempbuf, ntemp + 1); // + 1 to include the null terminator
        } else {
            memcpy(buf, tempbuf, bufsize - 1);
            buf[bufsize - 1] = '\0'; // because truncation removed null terminator
        }
    }
    return ntemp;
}

int snprintf(char *buf, size_t bufsize, const char *format, ...)
{ 
    va_list ap;
    va_start(ap, format);
    int totalChar = vsnprintf(buf, bufsize, format, ap);
    va_end(ap);
    return totalChar; 
}

int printf(const char *format, ...)
{
    char buf[MAX_OUTPUT_LEN];
    size_t bufsize = MAX_OUTPUT_LEN;
    va_list ap;
    va_start(ap, format);
    int totalChar = vsnprintf(buf, bufsize, format, ap);
    uart_putstring(buf);
    va_end(ap);
    return totalChar;
}
