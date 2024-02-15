/*
 * algorithms.c
 * ------------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This program implements the algorithms module to handle the retrieval, 
 * parsing, and converting of the commutator algorithms to be used by the robot.
 */

#include "algorithms.h"
#include "strings.h"
#include "malloc.h"
#include <stdarg.h>

#define SIZE_FACTOR 20
#define ROTATION_OFFSET 12
#define MAX_SOLVE_LEN 600
#define STD_MOVE_CUTOFF 18
#define min(x, y) ((x > y) ? y : x)
#define max(x, y) ((x > y) ? x : y)

const char std_moves[] = {'U','D','F','B','R','L','u','d','f','b','r','l','E','S','M','x','y','z'};
const char *comm_moves[] = {"U","U'","U2","D","D'","D2","F","F'","F2","B","B'","B2","R","R'","R2","L","L'","L2","y D","y' D'","y2 D2","y' U","y U'","y2 U2","z B","z' B'","z2 B2","z' F","z F'","z2 F2","x L","x' L'","x2 L2","x' R","x R'","x2 R2","y' U D'","y U' D","y2 U2 D2","z F' B","z' F B'","z2 F2 B2","x' R L'","x R' L","x2 R2 L2","x","x'","x2","y","y'","y2","z","z'","z2"};
const int x_rot[] = {comm_U, comm_U_prime, comm_U2, comm_y, comm_y2, comm_x, comm_R, comm_R_prime, comm_R2, comm_L, comm_L_prime, comm_L2, comm_F, comm_F_prime, comm_F2, comm_z, comm_z2, comm_x2, comm_R, comm_R_prime, comm_R2, comm_L, comm_L_prime, comm_L2, comm_D, comm_D_prime, comm_D2, comm_y_prime, comm_y2, comm_x_prime, comm_R, comm_R_prime, comm_R2, comm_L, comm_L_prime, comm_L2, comm_B, comm_B_prime, comm_B2, comm_z_prime, comm_z2, comm_x2, comm_R, comm_R_prime, comm_R2, comm_L, comm_L_prime, comm_L2};
const int y_rot[] = {comm_F, comm_F_prime, comm_F2, comm_x, comm_x2, comm_y, comm_U, comm_U_prime, comm_U2, comm_D, comm_D_prime, comm_D2, comm_R, comm_R_prime, comm_R2, comm_z_prime, comm_z2, comm_y2, comm_U, comm_U_prime, comm_U2, comm_D, comm_D_prime, comm_D2, comm_B, comm_B_prime, comm_B2, comm_x_prime, comm_x2, comm_y_prime, comm_U, comm_U_prime, comm_U2, comm_D, comm_D_prime, comm_D2, comm_L, comm_L_prime, comm_L2, comm_z, comm_z2, comm_y2, comm_U, comm_U_prime, comm_U2, comm_D, comm_D_prime, comm_D2};
const int z_rot[] = {comm_R, comm_R_prime, comm_R2, comm_x, comm_x2, comm_z, comm_F, comm_F_prime, comm_F2, comm_B, comm_B_prime, comm_B2, comm_U, comm_U_prime, comm_U2, comm_y, comm_y2, comm_z2, comm_F, comm_F_prime, comm_F2, comm_B, comm_B_prime, comm_B2, comm_L, comm_L_prime, comm_L2, comm_x_prime, comm_x2, comm_z_prime, comm_F, comm_F_prime, comm_F2, comm_B, comm_B_prime, comm_B2, comm_D, comm_D_prime, comm_D2, comm_y_prime, comm_y2, comm_z2, comm_F, comm_F_prime, comm_F2, comm_B, comm_B_prime, comm_B2};

static int get_next_move(char *comm) {
    int next_move = 0;
    char first = comm[0];
    char second = comm[1];
    
    for (int i = 0; i < sizeof(std_moves); i++) {
        if (first == std_moves[i]) {
            next_move += (3 * i);
            break;
        }
    }
    switch (second) {
        case '\'': next_move++;
            break;
        case '2': next_move += 2;
            break;
        default: break;
    }
    return next_move;
}

static int abs(int num) {
    return num >= 0 ? num : -num;
}

// gets complementary move to num
static int get_complement(int num) {
    if (num < comm_first || num > comm_last) return -1;
    else if (num % 3 == 0) return num + 1;
    else if (num % 3 == 1) return num - 1;
    return num;
}

static int find(const int *arr, int val, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == val) return i;
    }
    return -1;
}

static bool fully_refined(char *alg) {
    int index = 0;

    while(index < strlen(alg)) {
        int next = get_next_move((char *)&alg[index]);
        
        if (next >= comm_x) return false;
        index += 2;
        if (next % 3 != 0) index++;
    }
    return true;
}

bool are_opposites(int move1, int move2) { 
    return (max(move1, move2) < STD_MOVE_CUTOFF) && (min(move1 - move1 % 3, move2 - move2 % 3) % 6 == 0) && 
           (abs((move1 - move1 % 3) - (move2 - move2 % 3)) == 3);
}

// clears and frees memory given by variadic args
static void clear_memory(int size, ...) {
    va_list strings;

    va_start(strings, size);
    for (int i = 0; i < size; i++) {
        char *str = va_arg(strings, char *);

        memset(str, '\0', strlen(str));
        free(str);
    }
    va_end(strings);
}

char *invert_alg(const char *alg) { 
    int max_size = strlen(alg) * SIZE_FACTOR;
    char *inverted = malloc(max_size);
    memset(inverted, '\0', max_size);
    int index = 0;
    int reverse_index = strlen(alg) - 1;
    
    while (reverse_index >= 0) {
        for (int i = 0; i < sizeof(std_moves); i++) {
            if (alg[reverse_index] == std_moves[i]) {
                inverted[index] = alg[reverse_index];
                if (alg[reverse_index + 1] == ' ' || alg[reverse_index + 1] == '\0') {
                    index++;
                    inverted[index] = '\'';
                } else if (alg[reverse_index + 1] == '2') {
                    index++;
                    inverted[index] = '2';
                }
                index++;
                inverted[index] = ' ';
                index++;
                break;
            }
        }
        reverse_index--;
    }
    inverted[index] = '\0';
    return inverted;
}

char *parse_alg(const char *alg) {
    int index = 0;
    int max_size = strlen(alg) * SIZE_FACTOR;
    if (strlen(alg) == 0) return "";
    char *parsed = malloc(max_size);
    memset(parsed, '\0', max_size);

    while(index < strlen(alg)) {
        int next = get_next_move((char *)&alg[index]);
        
        strlcat(parsed, comm_moves[next], max_size);
        strlcat(parsed, " ", max_size);
        index += 2;
        if (next % 3 != 0) index++;
    }
    return parsed;
}

char *clear_rotations(const char* alg) {
    int shift = 0, curr_rot = 0;
    int max_size = strlen(alg) * SIZE_FACTOR;
    char *result = malloc(max_size);
    char *refined = malloc(max_size);
    memset(result, '\0', max_size);
    memset(refined, '\0', max_size);
     
    memcpy(result, alg, max_size);
    while (!fully_refined(result)) {
        int index = 0, next = 0;

        while (curr_rot == 0) {
            next = get_next_move((char *)&result[index]);
            if (next >= comm_x) {
                curr_rot = next;
                if (next % 3 == 0) shift = ROTATION_OFFSET;
                else if (next % 3 == 1) shift = -ROTATION_OFFSET;
                else shift = 2 * ROTATION_OFFSET;
            } else {
                strlcat(refined, comm_moves[next], max_size);
                strlcat(refined, " ", max_size);
            }
            index += 2;
            if (next % 3 != 0) index++;
        } 
        int num_duplicates = 0, count = 0;

        while (index < strlen(result)) {
            next = get_next_move((char *)&result[index]);
            if (curr_rot != 0 && next == get_complement(curr_rot) && count == num_duplicates) {
                curr_rot = 0;
                shift = 0;
                count = 0;
                num_duplicates = 0;
                index += 2;
                if (next % 3 != 0) index++;
                continue;
            } else if (curr_rot != 0 && (curr_rot - (curr_rot % 3)) <= next && next < (curr_rot + 3 - (curr_rot % 3))) {
                strlcat(refined, comm_moves[next], max_size);
                if (next == curr_rot) num_duplicates++;
                else if (next == get_complement(curr_rot)) count++;
            } else if (curr_rot >= comm_x && curr_rot <= comm_x2)
                strlcat(refined, comm_moves[x_rot[(find(x_rot, next, 48) + shift + 48) % 48]], max_size);
            else if (curr_rot >= comm_y && curr_rot <= comm_y2)
                strlcat(refined, comm_moves[y_rot[(find(y_rot, next, 48) + shift + 48) % 48]], max_size);
            else if (curr_rot >= comm_z && curr_rot <= comm_z2)
                strlcat(refined, comm_moves[z_rot[(find(z_rot, next, 48) + shift + 48) % 48]], max_size);
            else strlcat(refined, comm_moves[next], max_size);
            strlcat(refined, " ", max_size);
            index += 2;
            if (next % 3 != 0) index++;
        }
        refined[index] = '\0';
        memcpy(result, refined, max_size);
        memset(refined, '\0', max_size); 
    }
    clear_memory(1, refined);
    return result;
}

// Comms follow the form [A :[B, C]] = A B C B' C' A',
// where A is setup, B is comm_first, and C is comm_last.
// Alternatively, [A, B] = A B A' B' in the absence of setup.
// If param inverse is true, the results are of the form
// A C B C' B' A' or B A B' A'.
char *parse_comm(const char *comm, bool invert) {
    int index = 0;
    int max_size = strlen(comm) * SIZE_FACTOR;
    char *parsed = malloc(max_size);
    char *setup = malloc(max_size);
    char *comm_first = malloc(max_size);
    char *comm_last = malloc(max_size);

    memset(parsed, '\0', max_size);
    memset(setup, '\0', max_size);
    memset(comm_first, '\0', max_size);
    memset(comm_last, '\0', max_size);
    // valid comm
    if (comm[strlen(comm) - 1] == ']') {
        index++;
        int offset = 1;
        
        // deal with setup moves
        if (comm[strlen(comm) - 2] == ']') {
            while (comm[index] != ':') {
                setup[index - offset] = comm[index];
                index++;
            }
            setup[index - offset] = '\0';
            index += 2;
            offset += 2;
        } else setup[index - offset] = '\0'; // pure comm (no setup)
        while (comm[index] != ',') {
            comm_first[index - strlen(setup) - offset] = comm[index];
            index++;
        }
        comm_first[index - strlen(setup) - offset] = '\0';
        index += 2;
        offset += 2;
        while (comm[index] != ']') {
            comm_last[index - strlen(setup) - strlen(comm_first) - offset] = comm[index];
            index++;
        }
        comm_last[index - strlen(setup) - strlen(comm_first) - offset] = '\0';
    } else return (invert ? clear_rotations(invert_alg(parse_alg(comm))) : clear_rotations(parse_alg(comm))); // parse standard alg
    char *parsed_setup = parse_alg(setup);
    char *parsed_comm_first = parse_alg(comm_first);
    char *parsed_comm_last = parse_alg(comm_last);
    char *inverted_parsed_comm_first = invert_alg(parsed_comm_first);
    char *inverted_parsed_comm_last = invert_alg(parsed_comm_last);
    char *inverted_parsed_setup = invert_alg(parsed_setup);

    strlcat(parsed, parsed_setup, max_size);
    // if letter pair is inverted, swap order of comm_first and comm_last
    if (invert) {
        strlcat(parsed, parsed_comm_last, max_size);
        strlcat(parsed, parsed_comm_first, max_size);
        strlcat(parsed, inverted_parsed_comm_last, max_size);
        strlcat(parsed, inverted_parsed_comm_first, max_size);
    } else {
        strlcat(parsed, parsed_comm_first, max_size);
        strlcat(parsed, parsed_comm_last, max_size);
        strlcat(parsed, inverted_parsed_comm_first, max_size);
        strlcat(parsed, inverted_parsed_comm_last, max_size);
    }
    strlcat(parsed, inverted_parsed_setup, max_size);
    clear_memory(9, setup, comm_first, comm_last, parsed_setup, parsed_comm_first, parsed_comm_last, 
                 inverted_parsed_comm_first, inverted_parsed_comm_last, inverted_parsed_setup);
    return clear_rotations(parsed);
}

char *clear_duplicates(char *alg) {
    int alg_index = 0, /*double_last_index = 0,*/ last_index = 0, next = -1, prev = -1;//, double_prev = -1;
    int max_size = MAX_SOLVE_LEN;
    char *cleared = malloc(max_size);

    memset(cleared, '\0', max_size);
    while (alg_index < strlen(alg)) {
        next = get_next_move((char *)&alg[alg_index]); 
        if (next == get_complement(prev)) {
            cleared[last_index] = '\0';
            while (next == get_complement(prev) && last_index > 0) {
                last_index--;
                for (int i = 0; i < sizeof(std_moves); i++) {
                    if (cleared[last_index] == std_moves[i]) {
                        prev = get_next_move((char *)&cleared[last_index]);
                        break;
                    }
                }
            }
            alg_index += 2;
            if (next % 3 != 0) alg_index++;
            continue;
        } else if (next == prev) { 
            cleared[last_index] = '\0';
            //double_prev = prev;
            prev = next - next % 3 + 2;
            strlcat(cleared, comm_moves[next - next % 3 + 2], max_size);
        } else if (((next % 3 == 0) && (prev == next + 2)) || ((prev % 3 == 0) && (next == prev + 2))) {
            cleared[last_index] = '\0';
            //double_prev = prev;
            prev = next - next % 3 + 1;
            strlcat(cleared, comm_moves[next - next % 3 + 1], max_size); 
        } else if (((next % 3 == 1) && (prev == next + 1)) || ((prev % 3 == 1) && (next == prev + 1))) {
            cleared[last_index] = '\0';
            //double_prev = prev;
            prev = next - next % 3;
            strlcat(cleared, comm_moves[next - next % 3], max_size);
        } /*else if (are_opposites(next, prev) && (next - next % 3) == (double_prev - double_prev % 3)) {
            if (next == get_complement(double_prev)) {
                cleared[double_last_index] = '\0';
                while (next == get_complement(double_prev) && double_last_index > 0) {
                    double_last_index--;
                    for (int i = 0; i < sizeof(std_moves); i++) {
                        if (cleared[double_last_index] == std_moves[i]) {
                            double_prev = get_next_move((char *)&cleared[double_last_index]);
                            break;
                        }
                    }
                }
                strlcat(cleared, comm_moves[prev], max_size);
                strlcat(cleared, " ", max_size);
                last_index = double_last_index;
                bool marker_found = false;

                while (!marker_found && double_last_index > 0) {
                    double_last_index--;
                    for (int i = 0; i < sizeof(std_moves); i++) {
                        if (cleared[double_last_index] == std_moves[i]) {
                            marker_found = true;
                            break;
                        }
                    }
                }
                alg_index += 2;
                if (next % 3 != 0) alg_index++;
                continue;
            }
            else if (next == double_prev) {
                cleared[double_last_index] = '\0';
                double_prev = next - next % 3 + 2;
                strlcat(cleared, comm_moves[next - next % 3 + 2], max_size);
                if (next % 3 == 0) last_index++;
            } else if (((next % 3 == 0) && (double_prev == next + 2)) || ((double_prev % 3 == 0) && (next == double_prev + 2))) {
                cleared[double_last_index] = '\0';
                if (double_prev % 3 == 0) last_index++;
                double_prev = next - next % 3 + 1;
                strlcat(cleared, comm_moves[next - next % 3 + 1], max_size);
            } else if (((next % 3 == 1) && (double_prev == next + 1)) || ((double_prev % 3 == 1) && (next == double_prev + 1))) {
                cleared[double_last_index] = '\0';
                double_prev = next - next % 3;
                strlcat(cleared, comm_moves[next - next % 3], max_size);
                last_index--;
            }
            strlcat(cleared, " ", max_size);
            strlcat(cleared, comm_moves[prev], max_size);
        } */else {
            strlcat(cleared, comm_moves[next], max_size); 
            //double_last_index = last_index;
            last_index += 2;
            if (prev % 3 != 0) last_index++;
            //double_prev = prev;
            prev = next;
        }
        strlcat(cleared, " ", max_size);
        alg_index += 2;
        if (next % 3 != 0) alg_index++;
        if (strlen(cleared) <= 3) last_index = 0;
        //if (last_index <= 3) double_last_index = 0;
    }
    return cleared;
}
