/*
 * solver.c
 * --------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This program implements the cube solver using the 3-style blindfolded method.
 */

#include "solver.h"
#include "cube.h"
#include <stdbool.h>
#include "malloc.h"
#include "strings.h"

#define MAX_TRACE_CORNER_LEN 15
#define MAX_TRACE_EDGE_LEN 24
//1. Analyze cube using functions in cube.c to determine letter pairs
//2. From letter pairs, find the comms that we need.
//3. Parse those comms into strings readable by the robot.
//4. Concatenate those strings into one big "solve" string, cutting redundant moves.
//5. Return that string to the caller (cube_solver.c).

static bool parity;
static int visited_edges;
static const char edge_base_letters[] = {'A', 'B', 'C', 'D', 'J', 'L', 'R', 'T', 'U', 'V', 'W', 'X'};
static char visited_corners;
static const char corner_base_letters[] = {'A', 'B', 'C', 'D', 'U', 'V', 'W', 'X'};

void solve_init(void) {
    visited_corners = 4;
    visited_edges = 4;
    parity = false;
}

static void set_visited_edges(char cur_letter) {
    for (int i = 0; i < 12; i++) {
        if (cur_letter == edge_base_letters[i] || ref_edges[cur_letter - 65].neighbor == edge_base_letters[i]) {
            visited_edges |= (1 << i);
            return;
        }
    }
}

static void set_solved_edges(void) {
    for (int i = 0; i < 12; i++) {
        char cur_letter = edge_base_letters[i];
        char cur_color = get_color_edge_letter(cur_letter);
        char cur_color_nbr = get_color_edge_letter(get_neighbor_edge_letter(cur_letter));

        for (int j = A; j <= X; j++) {
            if (ref_edges[j].color == cur_color && ref_edges[ref_edges[j].neighbor - 65].color == cur_color_nbr) {
                if (parity && (j == B || j == C)) j = B + C - j;
                if (cur_letter == ref_edges[j].letter)
                    set_visited_edges(cur_letter);
                break;
            }
        } 
    }
}

char *trace_edges(void) {
    char *trace = malloc(MAX_TRACE_EDGE_LEN);
    int index = 0;
    char cur_letter = 'C';
    char cur_color, cur_color_nbr;
    char buf_letter = 'C';

    set_solved_edges();
    while (visited_edges != 0xFFF) { 
        cur_color = get_color_edge_letter(cur_letter);
        cur_color_nbr = get_color_edge_letter(get_neighbor_edge_letter(cur_letter));
        for (int i = A; i <= X; i++) {
            if (ref_edges[i].color == cur_color && ref_edges[ref_edges[i].neighbor - 65].color == cur_color_nbr) {
                if (parity && (i == B || i == C)) i = B + C - i;
                if (parity && (i == I || i == M)) i = I + M - i;
                if (ref_edges[i].letter == buf_letter || ref_edges[ref_edges[i].neighbor - 65].letter == buf_letter) {
                    if (ref_edges[i].letter != 'C' && ref_edges[i].letter != 'I') {
                        trace[index++] = ref_edges[i].letter;
                        set_visited_edges(ref_edges[i].letter);
                        if (visited_edges == 0xFFF) {
                            trace[index] = '\0';
                            return trace;
                        }
                    }
                    for (int i = 0; i < 12; i++) {
                        if ((visited_edges & (1 << i)) == 0) {
                            trace[index++] = edge_base_letters[i];
                            cur_letter = edge_base_letters[i];
                            buf_letter = edge_base_letters[i];
                            break;
                        }
                    }
                    break;
                }
                trace[index++] = ref_edges[i].letter;
                cur_letter = ref_edges[i].letter;
                set_visited_edges(ref_edges[i].letter);
                break;
            }
        }
    }
    trace[index] = '\0';
    return trace;
}

static void set_visited_corners(char cur_letter) {
    for (int i = 0; i < 8; i++) {
        if (cur_letter == corner_base_letters[i] || ref_corners[cur_letter - 65].neighbor_cw == corner_base_letters[i] || ref_corners[cur_letter - 65].neighbor_ccw == corner_base_letters[i]) {
            visited_corners |= (1 << i);
            return;
        }
    }
}

static void set_solved_corners(void) {
    for (int i = 0; i < 8; i++) {
        char cur_letter = corner_base_letters[i];
        char cur_color = get_color_corner_letter(cur_letter);
        char cur_color_cw = get_color_corner_letter(get_neighbor_cw_corner_letter(cur_letter));
        
        for (int j = A; j <= X; j++) {
            if (ref_corners[j].color == cur_color && ref_corners[ref_corners[j].neighbor_cw - 65].color == cur_color_cw) {
                if (cur_letter == ref_corners[j].letter)
                    set_visited_corners(cur_letter);
                break;
            }
        } 
    }
}

char *trace_corners(void) {
    char *trace = malloc(MAX_TRACE_CORNER_LEN);
    int index = 0;
    char cur_letter = 'C';
    char cur_color, cur_color_cw;
    char buf_letter = 'C';

    set_solved_corners();
    while (visited_corners != 0xFF) {
        cur_color = get_color_corner_letter(cur_letter);
        cur_color_cw = get_color_corner_letter(get_neighbor_cw_corner_letter(cur_letter));
        for (int i = A; i <= X; i++) {
            if (ref_corners[i].color == cur_color && ref_corners[ref_corners[i].neighbor_cw - 65].color == cur_color_cw) {
                if (ref_corners[i].letter == buf_letter || ref_corners[ref_corners[i].neighbor_cw - 65].letter == buf_letter || ref_corners[ref_corners[i].neighbor_ccw - 65].letter == buf_letter) {
                    if (ref_corners[i].letter != 'C' && ref_corners[i].letter != 'J' && ref_corners[i].letter != 'M') {
                        trace[index++] = ref_corners[i].letter;
                        set_visited_corners(ref_corners[i].letter);
                        if (visited_corners == 0xFF) {
                            trace[index] = '\0';
                            if (strlen(trace) % 2 == 1) parity = true; 
                            return trace;
                        }
                    }
                    for (int i = 0; i < 8; i++) {
                        if ((visited_corners & (1 << i)) == 0) {
                            trace[index++] = corner_base_letters[i];
                            cur_letter = corner_base_letters[i];
                            buf_letter = corner_base_letters[i];
                            break;
                        }
                    }
                    break;
                }
                trace[index++] = ref_corners[i].letter;
                cur_letter = ref_corners[i].letter;
                set_visited_corners(ref_corners[i].letter);
                break;
            }
        }
    }
    trace[index] = '\0';
    if (strlen(trace) % 2 == 1) parity = true; 
    return trace;
}
