/*
 * LARSON SCANNER EXTENSION
 * 
 * Author: Ethan Li
 *
 * This code modifies the blink.s code from Lab 1 
 * to create a Larson Scanner with several LEDs 
 * on at once, with varying brightnesses.
 *
 * This program uses GPIO20-27 for the scanner.
 *
 * To dim LEDs, lower ratio of time that they are turned on.
 *
 * Theoretically, according to how I defined my constants 
 * and structured my loops, the fully dim LEDs (center +- 2)
 * will be on 2/22 of the time, half dim LEDs (center +- 1) 
 * will be on (2+5)/22 = 7/22 of the time,
 * center LED will be on 22/22 of the time (continuously on).
 * 
 * Constants were adjusted to make 3 distinct levels of brightness 
 * and have a frequency of ~1Hz.
 */

.equ DELAY, 0x8F00
.equ NUM_LEDS, 8
.equ FULLDIM_ON, 0x2 // delay for fully dim LEDs to stay on
.equ HALFDIM_ON, 0x5 // delay for half dim LEDs to stay on AFTER fully dim LEDs turned off
.equ DIM_OFF, 0xF // delay for central LED to stay on before turning other LEDs back on

// configure GPIO 20-27 for output
ldr r0, FSEL2
mov r1, #1          //GPIO20
mov r2, #NUM_LEDS
subs r2, r2, #1
configLED:
    lsl r1, #3
    orr r1, r1, #1  //GPIO21-27
    subs r2, r2, #1
    bne configLED
str r1, [r0]

// Start center at GPIO20
mov r1, #(1<<20)         //GPIO20
orr r1, r1, #(1<<21)     //GPIO21
orr r1, r1, #(1<<22)     //GPIO22
orr r1, r1, #(1<<19)     //GPIO19
orr r1, r1, #(1<<18)     //GPIO18
// Full dim GPIOs (center +- 2)
mov r5, #(1<<22)
orr r5, r5, #(1<<18)
// Half dim GPIOs (center +- 1)
mov r6, #(1<<21)
orr r6, r6, #(1<<19)
// Reserve registers for SET0 and CLR0
ldr r0, SET0
ldr r4, CLR0

loop:

mov r3, #(NUM_LEDS-1) // distance from center to end of line for firstHalf

firstHalf:
// set current GPIOs low
str r1, [r4]

// shift
lsl r1, #1
lsl r5, #1
lsl r6, #1

// set next GPIOs high
str r1, [r0]

// delay
mov r2, #DELAY
wait1:
    str r1, [r0] // turn all 5 LEDs on
    mov r7, #FULLDIM_ON
    // timer for fully dim LEDs
    lit11:
        subs r7, r7, #1
        bne lit11
    str r5, [r4] // turn off fully dim LEDs
    mov r7, #HALFDIM_ON
    // timer for half dim LEDs
    lit12:
        subs r7, r7, #1
        bne lit12
    str r6, [r4] // turn off half dim LEDs
    mov r7, #DIM_OFF
    // timer for fully lit LED
    off1:
        subs r7, r7, #1
        bne off1
    subs r2, r2, #1
    bne wait1
subs r3, r3, #1
bne firstHalf // end loop when center reaches last LED

mov r3, #(NUM_LEDS-1) // reset distance from center to end of line for secondHalf

secondHalf:
// set current GPIOs low
str r1, [r4]

// shift
lsr r1, #1
lsr r5, #1
lsr r6, #1

// set next GPIOs high
str r1, [r0]

// delay
mov r2, #DELAY
wait2:
    str r1, [r0] // turn all 5 LEDs on
    mov r7, #FULLDIM_ON
    // timer for fully dim LEDs
    lit21:
        subs r7, r7, #1
        bne lit21
    str r5, [r4] // turn off fully dim LEDs
    mov r7, #HALFDIM_ON
    // timer for half dim LEDs
    lit22:
        subs r7, r7, #1
        bne lit22
    str r6, [r4] // turn off half dim LEDs
    mov r7, #DIM_OFF
    // timer for fully lit LED
    off2:
        subs r7, r7, #1
        bne off2
    subs r2, r2, #1
    bne wait2
subs r3, r3, #1
bne secondHalf // end loop when center reaches last LED

b loop

FSEL0: .word 0x20200000
FSEL1: .word 0x20200004
FSEL2: .word 0x20200008
SET0:  .word 0x2020001C
SET1:  .word 0x20200020
CLR0:  .word 0x20200028
CLR1:  .word 0x2020002C

