#ifndef CUBE_H
#define CUBE_H

/*
 * cube.h
 * ------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This module declares struct types for edge and corner letters, as well as an 
 * initial solved state of the cube.
 */

typedef struct {
    char letter; // letter of location
    char color; 
    char neighbor; // letter of neighbor edge sticker
} edge_t;

typedef struct {
    char letter; // letter of location
    char color;
    char neighbor_cw; // letter of neighbor in clockwise direction (e.g. CW neighbor of A is E)
    char neighbor_ccw;
} corner_t;

typedef enum {
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X
} letter_t;

void cube_init(void);

void print_cube_state();

char get_color_edge_letter(char letter);

char get_color_corner_letter(char letter);

char get_neighbor_edge_letter(char letter); 

char get_neighbor_cw_corner_letter(char letter);

char get_neighbor_ccw_corner_letter(char letter);

void u_move(int num_turns);
void d_move(int num_turns);
void r_move(int num_turns);
void l_move(int num_turns);
void f_move(int num_turns);
void b_move(int num_turns);

const edge_t ref_edges[24];
const corner_t ref_corners[24];

#endif
