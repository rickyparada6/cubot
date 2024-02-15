/*
 * malloc.c
 *
 * Author: Ethan Li
 * Assignment 4 Backtrace and Malloc
 *
 * This program implements a heap allocator to more efficiently allocate memory on the heap. It recycles freed memory when possible.
 */

#include "malloc.h"
#include "malloc_internal.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"

extern int __bss_end__;

#define STACK_START 0x8000000
#define STACK_SIZE  0x1000000
#define STACK_END ((char *)STACK_START - STACK_SIZE)

/*
 * The pool of memory available for the heap starts at the upper end of the 
 * data section and can extend from there up to the lower end of the stack.
 * It uses symbol __bss_end__ from memmap to locate data end
 * and calculates stack end assuming a 16MB stack.
 *
 * Global variables for the bump allocator:
 *
 * `heap_start`  location where heap segment starts
 * `heap_end`    location at end of in-use portion of heap segment 
 */

// Initial heap segment starts at bss_end and is empty
static void *heap_start = &__bss_end__;
static void *heap_end = &__bss_end__;

struct header {
    size_t payload_size;
    int status; // 0 if free, 1 if in-use
};

void *sbrk(int nbytes)
{
    void *prev_end = heap_end;
    if ((char *)prev_end + nbytes > STACK_END) {
        return NULL;
    } else {
        heap_end = (char *)prev_end + nbytes;
        return prev_end;
    }
}

// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

void *malloc (size_t nbytes)
{
    if (nbytes == 0) return NULL;
    nbytes = roundup(nbytes, 8);
    struct header *ptr = (struct header*)heap_start;
    while ((char*)ptr < (char*)heap_end) { // search for sufficient free blocks
        if (!(ptr->status) && ptr->payload_size >= nbytes) { // sufficient block found
            size_t prev_size = ptr->payload_size;
            ptr->payload_size = nbytes;
            ptr->status = 1;
            if (prev_size - nbytes >= 16) { // enough space for header + at least 8 bytes
                struct header *hdr = (struct header*)((char*)ptr + sizeof(struct header) + nbytes);
                hdr->payload_size = prev_size - nbytes - sizeof(struct header);
                hdr->status = 0;
            } else {
                ptr->payload_size = prev_size;
            }
            return (void*)((char*)ptr + sizeof(struct header)); 
        }
        // else keep searching
        ptr = (struct header*)((char*)ptr + sizeof(struct header) + ptr->payload_size);
    }
    // no sufficient block found
    struct header *hdr = (struct header*)sbrk(sizeof(struct header));
    if (hdr) {
        hdr->payload_size = nbytes;
        hdr->status = 1;
        return sbrk(nbytes);
    } else return NULL;
}

void free (void *ptr)
{
    if (!ptr) return;
    struct header *hdr = (struct header*)((char*)ptr - sizeof(struct header));
    hdr->status = 0;
    struct header *scoutptr = (struct header*)((char*)hdr + sizeof(struct header) + hdr->payload_size);
    while (!(scoutptr->status) && (char*)scoutptr < (char*)heap_end) {
        hdr->payload_size += scoutptr->payload_size + sizeof(struct header);
        scoutptr = (struct header*)((char*)scoutptr + sizeof(struct header) + scoutptr->payload_size);
    }
}

void *realloc (void *orig_ptr, size_t new_size)
{
    if (!orig_ptr) return malloc(new_size);
    if (new_size == 0) {
        free(orig_ptr);
        return NULL;
    }
    struct header *hdr = (struct header*)((char*)orig_ptr - sizeof(struct header));
    size_t prev_size = hdr->payload_size;
    
    if (new_size == prev_size) return orig_ptr;
    else if (new_size > prev_size) { // need to expand
        size_t total_size = prev_size;
        struct header *scoutptr = (struct header*)((char*)hdr + sizeof(struct header) + prev_size);
        while (!scoutptr->status && (char*)scoutptr < (char*)heap_end) { // keep combining free forward neighbors until sufficient or can't
            total_size += sizeof(struct header) + scoutptr->payload_size;
            if (total_size > new_size) {
                hdr->payload_size = new_size;
                if (total_size - new_size >= 16) { // enough space for header + at least 8 bytes
                    struct header *new_hdr = (struct header*)((char*)hdr + sizeof(struct header) + new_size);
                    new_hdr->payload_size = total_size - new_size - sizeof(struct header);
                    new_hdr->status = 0;
                } else hdr->payload_size = total_size;
                return orig_ptr;
            } else scoutptr = (struct header*)((char*)hdr + sizeof(struct header) + total_size);
        }
        if ((char*)scoutptr == (char*)heap_end) { // search reached end of heap, safe to realloc in-place
            hdr->payload_size = new_size;
            heap_end = (char*)heap_end + new_size - total_size;
            return orig_ptr;
        }
        // not enough space to realloc in-place
        void *new_ptr = malloc(new_size);
        if (!new_ptr) return NULL;
        memcpy(new_ptr, orig_ptr, prev_size);
        free(orig_ptr);
        return new_ptr;
    } else { // need to shrink
        hdr->payload_size = new_size;
        if (prev_size - new_size >= 16) { // enough space for header + at least8 bytes
                struct header *new_hdr = (struct header*)((char*)hdr + sizeof(struct header) + new_size);
                new_hdr->payload_size = prev_size - new_size - sizeof(struct header);
                new_hdr->status = 0;
        } else hdr->payload_size = prev_size;
        return orig_ptr;  
    }
}

void heap_dump (const char *label)
{
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", heap_start, heap_end);
    struct header *ptr = (struct header *)heap_start;
    while ((char*)ptr < (char*)heap_end) {
        printf("Payload size: %x, Status: %d, First byte: %x\n", ptr->payload_size, ptr->status, (unsigned char)*((char*)ptr + sizeof(struct header)));
        ptr = (struct header*)((char*)ptr + sizeof(struct header) + ptr->payload_size);
    }
    printf("----------  END DUMP (%s) ----------\n", label);
}

void memory_report (void)
{
    printf("\n=============================================\n");
    printf(  "         Mini-Valgrind Memory Report         \n");
    printf(  "=============================================\n");
    // TODO: fill in for extension
}
