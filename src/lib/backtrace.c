/*
 * backtrace.c
 *
 * Author: Ethan Li
 * Assignment 4 Backtrace and Malloc
 *
 * This program implements a backtrace module which allows the user to gather and print a stack backtrace.
 */

#include "backtrace.h"
#include "printf.h"

const char *name_of(uintptr_t fn_start_addr)
{
    if ((*(unsigned int*)(fn_start_addr - sizeof(uintptr_t)) & (0xFF << 24)) == (0xFF << 24)) 
        return (const char*)(fn_start_addr - sizeof(uintptr_t) - (*(unsigned int*)(fn_start_addr - sizeof(uintptr_t)) & ~(0xFF << 24)));
    else 
        return "???";
}

int backtrace (frame_t f[], int max_frames)
{    
    int frame = 0;    
    unsigned int *cur_fp;
    __asm__("mov %0, fp" : "=r" (cur_fp));
    uintptr_t res_addr = (uintptr_t)(*(cur_fp - 1)); // saved lr
    cur_fp = (unsigned int*)(*(cur_fp - 3)); // switch to prev stack frame
    uintptr_t fn_start_addr = NULL;
    int res_offset = 0;
    while (cur_fp && frame < max_frames) { 
        fn_start_addr = (uintptr_t)(*cur_fp - 12); // saved pc - 12 holds addr of first instruction of func
        res_offset = res_addr - fn_start_addr; 
        
        f[frame].name = name_of(fn_start_addr);
        f[frame].resume_addr = res_addr;
        f[frame].resume_offset = res_offset;
        frame++;

        res_addr = (uintptr_t)(*(cur_fp - 1)); // saved lr
        cur_fp = (unsigned int*)(*(cur_fp - 3)); // switch to prev stack frame
    }
    return frame;
}

void print_frames (frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

void print_backtrace (void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
