/*
 * strings.c
 *
 * Author: Ethan Li
 * Assignment 3 Implement a String Formatting Library
 *
 * This program implements a pared-down version of the strings module.
 *
 * Function descriptions are in the strings.h file.
 */

#include "strings.h"

void *memset(void *s, int c, size_t n)
{
    char *s1 = (char *)s;
    for (int i = 0; i < n; i++) {
        s1[i] = (unsigned char)c;
    }
    return s1;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    char *dst1 = (char *)dst;
    char *src1 = (char *)src;
    for (int i = 0; i < n; i++) {
        dst1[i] = src1[i];
    }
    return dst1;
}

size_t strlen(const char *s)
{
    /* Implementation a gift to you from lab3 */
    int n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

int strcmp(const char *s1, const char *s2)
{
    int n = 0;
    while (s1[n] != '\0' && s2[n] != '\0') {
        if (s1[n] < s2[n]) {
            return -(s2[n] - s1[n]);
        } else if (s1[n] > s2[n]) {
            return s1[n] - s2[n];
        } else {
            n++;
        }
    }
    if (s1[n] == '\0' && s2[n] == '\0') {
        return 0;
    } else if (s1[n] == '\0') {
        return -s2[n];
    } else {
        return s1[n];
    }
}

size_t strlcat(char *dst, const char *src, size_t maxsize)
{
    int dstlen = strlen(dst);
    int n = dstlen;
    int i = 0;
    while (n < maxsize - 1 && src[i] != '\0') {
        dst[n] = src[i];
        n++;
        i++;
    }
    dst[n] = '\0';
    return dstlen + strlen(src);
}

unsigned int strtonum(const char *str, const char **endptr)
{
    unsigned int result = 0;
    int n = 0;
    int base = 10;
    if (str[0] == '0' && str[1] == 'x') {
        base = 16;
        n = 2;
    }
    while ((str[n] >= 48 && str[n] <= 57) || (str[n] >= 97 && str[n] <= 102)) {
        if (base == 10 && str[n] >= 97 && str[n] <= 102) {
            break;
        } else if (str[n] >= 97 && str[n] <= 102) {
            result = (result * base) + str[n] - 87;
        } else {
            result = (result * base) + str[n] - 48;
        }
        n++;
    }
    if (endptr != NULL) {
        *endptr = &str[n];
    }
    return result;
}
