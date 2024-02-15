#ifndef ALGORITHMS_H
#define ALGORITHMS_H

/*
 * algorithms.h
 * ------------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This module defines algorithm manipulation for the 3-style 
 * blindfolded method, to be used by the robot.
 */
#include <stdbool.h>

typedef enum {
    comm_first = 0,
    comm_U = 0, //std moves
    comm_U_prime,
    comm_U2,
    comm_D,
    comm_D_prime,
    comm_D2,
    comm_F,
    comm_F_prime,
    comm_F2,
    comm_B,
    comm_B_prime,
    comm_B2,
    comm_R,
    comm_R_prime,
    comm_R2,
    comm_L,
    comm_L_prime,
    comm_L2,
    comm_u, //wide moves
    comm_u_prime,
    comm_u2,
    comm_d,
    comm_d_prime,
    comm_d2,
    comm_f,
    comm_f_prime,
    comm_f2,
    comm_b,
    comm_b_prime,
    comm_b2,
    comm_r,
    comm_r_prime,
    comm_r2,
    comm_l,
    comm_l_prime,
    comm_l2,
    comm_E, // slice moves
    comm_E_prime,
    comm_E2,
    comm_S,
    comm_S_prime,
    comm_S2,
    comm_M,
    comm_M_prime,
    comm_M2,
    comm_x, // rotation moves
    comm_x_prime,
    comm_x2,
    comm_y,
    comm_y_prime,
    comm_y2,
    comm_z,
    comm_z_prime,
    comm_z2,
    comm_last
} comm_moves_t;

const char std_moves[18];
const char *comm_moves[54];
const int x_rot[48];
const int y_rot [48];
const int z_rot[48];

bool are_opposites(int move1, int move2);
char *invert_alg(const char *alg);
char *parse_alg(const char *alg);
char *clear_rotations(const char *alg);
char *parse_comm(const char *comm, bool invert);
char *clear_duplicates(char *alg);

#endif
