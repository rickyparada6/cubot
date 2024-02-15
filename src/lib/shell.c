/*
 * shell.c
 *
 * Author: Ethan Li
 * Assignment 5 Keyboard and Simple Shell
 *
 * This program implements a simple shell which allows the user to enter commands 
 * and control the RPi.
 */

#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "keyboard.h"
#include "strings.h"
#include "malloc.h"
#include "pi.h"

#define LINE_LEN 80

static formatted_fn_t shell_printf;

static const command_t commands[] = {
    {"help",   "<cmd> prints a list of commands or description of cmd", cmd_help},
    {"echo",   "<...> echos the user input to the screen", cmd_echo},
    {"reboot", "reboot the Raspberry Pi back to the bootloader", cmd_reboot},
    {"peek", "[address] print contents of memory at address", cmd_peek},
    {"poke", "[address] [value] store value at address", cmd_poke}
};

int cmd_echo(int argc, const char *argv[]) {
    for (int i = 1; i < argc; ++i) 
        shell_printf("%s ", argv[i]);
    shell_printf("\n");
    return 0;
}

int cmd_help(int argc, const char *argv[]) {
    if (argc == 1) { // no arguments
        for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
            shell_printf("%s: %s\n", commands[i].name, commands[i].description);
        }
        return 0;
    } else {
        for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
            if (strcmp(argv[1], commands[i].name) == 0) {
                shell_printf("%s: %s\n", commands[i].name, commands[i].description);
                return 0;
            }
        }
        shell_printf("error: no such command '%s'\n", argv[1]);
    }
    return 1;
}

int cmd_reboot(int argc, const char *argv[]) {
    shell_printf("See you later!\n");
    uart_putchar(EOT);
    pi_reboot();
    return 0;
}

int cmd_peek(int argc, const char *argv[]) {
    if (argc == 1) {
        shell_printf("error: peek expects 1 argument [address]\n");
        return 1;
    }

    const char *addr_string = argv[1]; // need to store separate string because strtonum overwrites for some reason
    const char **endptr = &argv[1];
    unsigned int *addr_num = (unsigned int*)strtonum(argv[1], endptr);

    if (**endptr != '\0') { // invalid argument, endptr doesn't point to null-terminator
        shell_printf("error: peek cannot convert '%s'\n", addr_string);
        return 1;
    } else if ((unsigned int)addr_num % 4) {
        shell_printf("error: peek address must be 4-byte aligned\n");
        return 1;
    } else {
        shell_printf("%p: %08x\n", addr_num, *addr_num);
    }
    return 0;
}

int cmd_poke(int argc, const char *argv[]) {
    if (argc < 3) {
        shell_printf("error: poke expects 2 arguments [address] [value]\n");
        return 1;
    }

    const char *addr_value_strings[] = {argv[1], argv[2]}; // need to store separate strings because strtonum overwrites for some reason
    const char **endptr[] = {&argv[1], &argv[2]};
    unsigned int *addr_num = (unsigned int*)strtonum(argv[1], endptr[0]);
    unsigned int value_num = strtonum(argv[2], endptr[1]);

    for (int i = 0; i < 2; i++) {
        if (**endptr[i] != '\0') { // invalid argument, endptr doesn't point to null-terminator
            shell_printf("error: poke cannot convert '%s'\n", addr_value_strings[i]);
            return 1;
        } 
    }

    if ((unsigned int)addr_num % 4) {
        shell_printf("error: poke address must be 4-byte aligned\n");
        return 1;
    } else {
        *addr_num = value_num;
    }
    return 0;
}

void shell_init(formatted_fn_t print_fn) {
    shell_printf = print_fn;
}

void shell_bell(void) {
    uart_putchar('\a');
}

void shell_readline(char buf[], size_t bufsize) {
    char c;
    int i = 0;
    while (1) {
        c = keyboard_read_next();
        if (c == '\n') {
            shell_printf("\n");
            buf[i] = '\0';
            break;
        } else if (c == '\b') {
            if (i == 0) {
                shell_bell();
                continue;
            } else {
                shell_printf("%c %c", '\b', '\b');
                i--;
            } 
        } else if (i >= bufsize - 1) {
            shell_bell();
            continue;
        } else {
            shell_printf("%c", c);
            buf[i++] = c;
        }
    }
}

/*
 * strndup()
 * --------
 * Returns a pointer to a null-terminated string containing at most 'n' bytes
 * copied from the string pointed to by 'src'.
 *
 * @param src: the source string
 * @param n: the maximum number of bytes to copy
 *
 * @return ptr: a pointer to the newly created string on the heap
 */
static char *strndup(const char *src, size_t n) {
    char *ptr = malloc(n + 1);
    ptr = memcpy(ptr, src, n);
    ptr[n] = '\0';
    return ptr;
}

/*
 * is_space()
 * ---------
 * Returns whether a character is whitespace.
 *
 * @param c: the given character
 *
 * @return int: whether the character is whitespace
 */
static int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

/*
 * tokenize()
 * ---------
 * Splits a string line into an array of tokens.
 *
 * @param line: the given string line
 * @param tokens: the array to contain the tokens
 * @param max_tokens: the maximum number of tokens to create
 *
 * @return ntokens: the number of tokens created
 */
static int tokenize(const char *line, char *tokens[], int max_tokens) {
    int ntokens = 0;
    const char *cur = line;
    while (ntokens < max_tokens) {
        while (is_space(*cur)) cur++; // ignore spaces
        if (*cur == '\0') break; // break at end of string
        const char *start = cur;
        while (*cur != '\0' && !is_space(*cur)) cur++; // advance until space or end
        tokens[ntokens++] = strndup(start, cur - start);
    }
    return ntokens;
}

int shell_evaluate(const char *line) {
    int success = 0;
    int found = 0;
    int max_tokens = strlen(line);
    char *tokens[max_tokens];
    int num_tokens = tokenize(line, tokens, max_tokens);

    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (strcmp(tokens[0], commands[i].name) == 0) {
            success = (commands[i].fn)(num_tokens, (const char**)tokens);
            found = 1;
            break;
        }
    }
    if (!found) {
        shell_printf("error: no such command '%s'\n", tokens[0]);
        success = 1;
    }
    for (int i = 0; i < num_tokens; i++) free(tokens[i]);
    return success;
}

void shell_run(void) {
    shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");
    while (1) 
    {
        char line[LINE_LEN];

        shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}
