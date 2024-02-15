/*
 * cube.c
 * ------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This program implements a cube module which contains the current representation 
 * of the Rubik's Cube and allows standard moves (U, D, R, L, F, B) to be performed on the cube.
 */

#include "cube.h"
#include "algorithms.h"
#include "printf.h"

const edge_t ref_edges[] = {
    {'A', 'W', 'Q'},
    {'B', 'W', 'M'},
    {'C', 'W', 'I'},
    {'D', 'W', 'E'},
    {'E', 'O', 'D'},
    {'F', 'O', 'L'},
    {'G', 'O', 'X'},
    {'H', 'O', 'R'},
    {'I', 'G', 'C'},
    {'J', 'G', 'P'},
    {'K', 'G', 'U'},
    {'L', 'G', 'F'},
    {'M', 'R', 'B'},
    {'N', 'R', 'T'},
    {'O', 'R', 'V'},
    {'P', 'R', 'J'},
    {'Q', 'B', 'A'},
    {'R', 'B', 'H'},
    {'S', 'B', 'W'},
    {'T', 'B', 'N'},
    {'U', 'Y', 'K'},
    {'V', 'Y', 'O'},
    {'W', 'Y', 'S'},
    {'X', 'Y', 'G'}
};

const corner_t ref_corners[] = {
    {'A', 'W', 'E', 'R'},
    {'B', 'W', 'Q', 'N'},
    {'C', 'W', 'M', 'J'},
    {'D', 'W', 'I', 'F'},
    {'E', 'O', 'R', 'A'},
    {'F', 'O', 'D', 'I'},
    {'G', 'O', 'L', 'U'},
    {'H', 'O', 'X', 'S'},
    {'I', 'G', 'F', 'D'},
    {'J', 'G', 'C', 'M'},
    {'K', 'G', 'P', 'V'},
    {'L', 'G', 'U', 'G'},
    {'M', 'R', 'J', 'C'},
    {'N', 'R', 'B', 'Q'},
    {'O', 'R', 'T', 'W'},
    {'P', 'R', 'V', 'K'},
    {'Q', 'B', 'N', 'B'},
    {'R', 'B', 'A', 'E'},
    {'S', 'B', 'H', 'X'},
    {'T', 'B', 'W', 'O'},
    {'U', 'Y', 'G', 'L'},
    {'V', 'Y', 'K', 'P'},
    {'W', 'Y', 'O', 'T'},
    {'X', 'Y', 'S', 'H'}
};

static edge_t cube_edges[sizeof(ref_edges) / sizeof(ref_edges[0])];
static corner_t cube_corners[sizeof(ref_corners) / sizeof(ref_corners[0])];

void cube_init(void) {
    for (int i = A; i <= X; i++) {
        cube_edges[i] = ref_edges[i];
        cube_corners[i] = ref_corners[i];
    }
}

void print_cube_state() {
    printf("      %c %c %c\n", cube_corners[A].color, cube_edges[A].color, cube_corners[B].color);
    printf("      %c W %c\n", cube_edges[D].color, cube_edges[B].color);
    printf("      %c %c %c\n", cube_corners[D].color, cube_edges[C].color, cube_corners[C].color);
    printf("%c %c %c %c %c %c %c %c %c %c %c %c\n", cube_corners[E].color, cube_edges[E].color, cube_corners[F].color, cube_corners[I].color, cube_edges[I].color, cube_corners[J].color, cube_corners[M].color, cube_edges[M].color, cube_corners[N].color, cube_corners[Q].color, cube_edges[Q].color, cube_corners[R].color);
    printf("%c O %c %c G %c %c R %c %c B %c\n", cube_edges[H].color, cube_edges[F].color, cube_edges[L].color, cube_edges[J].color, cube_edges[P].color, cube_edges[N].color, cube_edges[T].color, cube_edges[R].color); 
    printf("%c %c %c %c %c %c %c %c %c %c %c %c\n", cube_corners[H].color, cube_edges[G].color, cube_corners[G].color, cube_corners[L].color, cube_edges[K].color, cube_corners[K].color, cube_corners[P].color, cube_edges[O].color, cube_corners[O].color, cube_corners[T].color, cube_edges[S].color, cube_corners[S].color);
    printf("      %c %c %c\n", cube_corners[U].color, cube_edges[U].color, cube_corners[V].color);
    printf("      %c Y %c\n", cube_edges[X].color, cube_edges[V].color);
    printf("      %c %c %c\n", cube_corners[X].color, cube_edges[W].color, cube_corners[W].color);
}

char get_color_edge_letter(char letter) {
    return cube_edges[letter - 65].color; // 'A' is ascii 0d65
}

char get_color_corner_letter(char letter) {
    return cube_corners[letter - 65].color;
}

char get_neighbor_edge_letter(char letter) {
    return cube_edges[letter - 65].neighbor;
}

char get_neighbor_cw_corner_letter(char letter) {
    return cube_corners[letter - 65].neighbor_cw;
}

char get_neighbor_ccw_corner_letter(char letter) {
    return cube_corners[letter - 65].neighbor_ccw;
}

void u_move(int num_turns) {
    edge_t *u_face_edges[] = {&cube_edges[D], &cube_edges[C], &cube_edges[B], &cube_edges[A]};
    char u_face_edge_colors[] = {cube_edges[D].color, cube_edges[C].color, cube_edges[B].color, cube_edges[A].color};
    for (int i = 0; i < 4; i++) {
        u_face_edges[i]->color = u_face_edge_colors[(i + num_turns) % 4];
    }

    edge_t *other_face_edges[] = {&cube_edges[E], &cube_edges[I], &cube_edges[M], &cube_edges[Q]};
    char other_face_edge_colors[] = {cube_edges[E].color, cube_edges[I].color, cube_edges[M].color, cube_edges[Q].color};
    for (int i = 0; i < 4; i++) {
        other_face_edges[i]->color = other_face_edge_colors[(i + num_turns) % 4];
    }

    corner_t *u_face_corners[] = {&cube_corners[D], &cube_corners[C], &cube_corners[B], &cube_corners[A]};
    char u_face_corner_colors[] = {cube_corners[D].color, cube_corners[C].color, cube_corners[B].color, cube_corners[A].color};
    for (int i = 0; i < 4; i++) {
        u_face_corners[i]->color = u_face_corner_colors[(i + num_turns) % 4];
    }

    corner_t *other_face_corners_cw[] = {&cube_corners[I], &cube_corners[M], &cube_corners[Q], &cube_corners[E]};
    char other_face_corner_cw_colors[] = {cube_corners[I].color, cube_corners[M].color, cube_corners[Q].color, cube_corners[E].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_cw[i]->color = other_face_corner_cw_colors[(i + num_turns) % 4];
    }
    
    corner_t *other_face_corners_ccw[] = {&cube_corners[F], &cube_corners[J], &cube_corners[N], &cube_corners[R]};
    char other_face_corner_ccw_colors[] = {cube_corners[F].color, cube_corners[J].color, cube_corners[N].color, cube_corners[R].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_ccw[i]->color = other_face_corner_ccw_colors[(i + num_turns) % 4];
    }
}

void d_move(int num_turns) {
    edge_t *d_face_edges[] = {&cube_edges[X], &cube_edges[W], &cube_edges[V], &cube_edges[U]};
    char d_face_edge_colors[] = {cube_edges[X].color, cube_edges[W].color, cube_edges[V].color, cube_edges[U].color};
    for (int i = 0; i < 4; i++) {
        d_face_edges[i]->color = d_face_edge_colors[(i + num_turns) % 4];
    }

    edge_t *other_face_edges[] = {&cube_edges[G], &cube_edges[S], &cube_edges[O], &cube_edges[K]};
    char other_face_edge_colors[] = {cube_edges[G].color, cube_edges[S].color, cube_edges[O].color, cube_edges[K].color};
    for (int i = 0; i < 4; i++) {
        other_face_edges[i]->color = other_face_edge_colors[(i + num_turns) % 4];
    }

    corner_t *d_face_corners[] = {&cube_corners[X], &cube_corners[W], &cube_corners[V], &cube_corners[U]};
    char d_face_corner_colors[] = {cube_corners[X].color, cube_corners[W].color, cube_corners[V].color, cube_corners[U].color};
    for (int i = 0; i < 4; i++) {
        d_face_corners[i]->color = d_face_corner_colors[(i + num_turns) % 4];
    }

    corner_t *other_face_corners_cw[] = {&cube_corners[S], &cube_corners[O], &cube_corners[K], &cube_corners[G]};
    char other_face_corner_cw_colors[] = {cube_corners[S].color, cube_corners[O].color, cube_corners[K].color, cube_corners[G].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_cw[i]->color = other_face_corner_cw_colors[(i + num_turns) % 4];
    }
    
    corner_t *other_face_corners_ccw[] = {&cube_corners[H], &cube_corners[T], &cube_corners[P], &cube_corners[L]};
    char other_face_corner_ccw_colors[] = {cube_corners[H].color, cube_corners[T].color, cube_corners[P].color, cube_corners[L].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_ccw[i]->color = other_face_corner_ccw_colors[(i + num_turns) % 4];
    }
}

void r_move(int num_turns) {
    edge_t *r_face_edges[] = {&cube_edges[P], &cube_edges[O], &cube_edges[N], &cube_edges[M]};
    char r_face_edge_colors[] = {cube_edges[P].color, cube_edges[O].color, cube_edges[N].color, cube_edges[M].color};
    for (int i = 0; i < 4; i++) {
        r_face_edges[i]->color = r_face_edge_colors[(i + num_turns) % 4];
    }

    edge_t *other_face_edges[] = {&cube_edges[J], &cube_edges[V], &cube_edges[T], &cube_edges[B]};
    char other_face_edge_colors[] = {cube_edges[J].color, cube_edges[V].color, cube_edges[T].color, cube_edges[B].color};
    for (int i = 0; i < 4; i++) {
        other_face_edges[i]->color = other_face_edge_colors[(i + num_turns) % 4];
    }

    corner_t *r_face_corners[] = {&cube_corners[P], &cube_corners[O], &cube_corners[N], &cube_corners[M]};
    char r_face_corner_colors[] = {cube_corners[P].color, cube_corners[O].color, cube_corners[N].color, cube_corners[M].color};
    for (int i = 0; i < 4; i++) {
        r_face_corners[i]->color = r_face_corner_colors[(i + num_turns) % 4];
    }

    corner_t *other_face_corners_cw[] = {&cube_corners[V], &cube_corners[T], &cube_corners[B], &cube_corners[J]};
    char other_face_corner_cw_colors[] = {cube_corners[V].color, cube_corners[T].color, cube_corners[B].color, cube_corners[J].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_cw[i]->color = other_face_corner_cw_colors[(i + num_turns) % 4];
    }
    
    corner_t *other_face_corners_ccw[] = {&cube_corners[K], &cube_corners[W], &cube_corners[Q], &cube_corners[C]};
    char other_face_corner_ccw_colors[] = {cube_corners[K].color, cube_corners[W].color, cube_corners[Q].color, cube_corners[C].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_ccw[i]->color = other_face_corner_ccw_colors[(i + num_turns) % 4];
    }
}

void l_move(int num_turns) {
    edge_t *l_face_edges[] = {&cube_edges[H], &cube_edges[G], &cube_edges[F], &cube_edges[E]};
    char l_face_edge_colors[] = {cube_edges[H].color, cube_edges[G].color, cube_edges[F].color, cube_edges[E].color};
    for (int i = 0; i < 4; i++) {
        l_face_edges[i]->color = l_face_edge_colors[(i + num_turns) % 4];
    }

    edge_t *other_face_edges[] = {&cube_edges[R], &cube_edges[X], &cube_edges[L], &cube_edges[D]};
    char other_face_edge_colors[] = {cube_edges[R].color, cube_edges[X].color, cube_edges[L].color, cube_edges[D].color};
    for (int i = 0; i < 4; i++) {
        other_face_edges[i]->color = other_face_edge_colors[(i + num_turns) % 4];
    }

    corner_t *l_face_corners[] = {&cube_corners[H], &cube_corners[G], &cube_corners[F], &cube_corners[E]};
    char l_face_corner_colors[] = {cube_corners[H].color, cube_corners[G].color, cube_corners[F].color, cube_corners[E].color};
    for (int i = 0; i < 4; i++) {
        l_face_corners[i]->color = l_face_corner_colors[(i + num_turns) % 4];
    }

    corner_t *other_face_corners_cw[] = {&cube_corners[X], &cube_corners[L], &cube_corners[D], &cube_corners[R]};
    char other_face_corner_cw_colors[] = {cube_corners[X].color, cube_corners[L].color, cube_corners[D].color, cube_corners[R].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_cw[i]->color = other_face_corner_cw_colors[(i + num_turns) % 4];
    }
    
    corner_t *other_face_corners_ccw[] = {&cube_corners[S], &cube_corners[U], &cube_corners[I], &cube_corners[A]};
    char other_face_corner_ccw_colors[] = {cube_corners[S].color, cube_corners[U].color, cube_corners[I].color, cube_corners[A].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_ccw[i]->color = other_face_corner_ccw_colors[(i + num_turns) % 4];
    }
}

void f_move(int num_turns) {
    edge_t *f_face_edges[] = {&cube_edges[L], &cube_edges[K], &cube_edges[J], &cube_edges[I]};
    char f_face_edge_colors[] = {cube_edges[L].color, cube_edges[K].color, cube_edges[J].color, cube_edges[I].color};
    for (int i = 0; i < 4; i++) {
        f_face_edges[i]->color = f_face_edge_colors[(i + num_turns) % 4];
    }

    edge_t *other_face_edges[] = {&cube_edges[F], &cube_edges[U], &cube_edges[P], &cube_edges[C]};
    char other_face_edge_colors[] = {cube_edges[F].color, cube_edges[U].color, cube_edges[P].color, cube_edges[C].color};
    for (int i = 0; i < 4; i++) {
        other_face_edges[i]->color = other_face_edge_colors[(i + num_turns) % 4];
    }

    corner_t *f_face_corners[] = {&cube_corners[L], &cube_corners[K], &cube_corners[J], &cube_corners[I]};
    char f_face_corner_colors[] = {cube_corners[L].color, cube_corners[K].color, cube_corners[J].color, cube_corners[I].color};
    for (int i = 0; i < 4; i++) {
        f_face_corners[i]->color = f_face_corner_colors[(i + num_turns) % 4];
    }

    corner_t *other_face_corners_cw[] = {&cube_corners[U], &cube_corners[P], &cube_corners[C], &cube_corners[F]};
    char other_face_corner_cw_colors[] = {cube_corners[U].color, cube_corners[P].color, cube_corners[C].color, cube_corners[F].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_cw[i]->color = other_face_corner_cw_colors[(i + num_turns) % 4];
    }
    
    corner_t *other_face_corners_ccw[] = {&cube_corners[G], &cube_corners[V], &cube_corners[M], &cube_corners[D]};
    char other_face_corner_ccw_colors[] = {cube_corners[G].color, cube_corners[V].color, cube_corners[M].color, cube_corners[D].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_ccw[i]->color = other_face_corner_ccw_colors[(i + num_turns) % 4];
    }
}

void b_move(int num_turns) {
    edge_t *b_face_edges[] = {&cube_edges[T], &cube_edges[S], &cube_edges[R], &cube_edges[Q]};
    char b_face_edge_colors[] = {cube_edges[T].color, cube_edges[S].color, cube_edges[R].color, cube_edges[Q].color};
    for (int i = 0; i < 4; i++) {
        b_face_edges[i]->color = b_face_edge_colors[(i + num_turns) % 4];
    }

    edge_t *other_face_edges[] = {&cube_edges[N], &cube_edges[W], &cube_edges[H], &cube_edges[A]};
    char other_face_edge_colors[] = {cube_edges[N].color, cube_edges[W].color, cube_edges[H].color, cube_edges[A].color};
    for (int i = 0; i < 4; i++) {
        other_face_edges[i]->color = other_face_edge_colors[(i + num_turns) % 4];
    }

    corner_t *b_face_corners[] = {&cube_corners[T], &cube_corners[S], &cube_corners[R], &cube_corners[Q]};
    char b_face_corner_colors[] = {cube_corners[T].color, cube_corners[S].color, cube_corners[R].color, cube_corners[Q].color};
    for (int i = 0; i < 4; i++) {
        b_face_corners[i]->color = b_face_corner_colors[(i + num_turns) % 4];
    }

    corner_t *other_face_corners_cw[] = {&cube_corners[W], &cube_corners[H], &cube_corners[A], &cube_corners[N]};
    char other_face_corner_cw_colors[] = {cube_corners[W].color, cube_corners[H].color, cube_corners[A].color, cube_corners[N].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_cw[i]->color = other_face_corner_cw_colors[(i + num_turns) % 4];
    }
    
    corner_t *other_face_corners_ccw[] = {&cube_corners[O], &cube_corners[X], &cube_corners[E], &cube_corners[B]};
    char other_face_corner_ccw_colors[] = {cube_corners[O].color, cube_corners[X].color, cube_corners[E].color, cube_corners[B].color};
    for (int i = 0; i < 4; i++) {
        other_face_corners_ccw[i]->color = other_face_corner_ccw_colors[(i + num_turns) % 4];
    }
}


