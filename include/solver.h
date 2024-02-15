#ifndef SOLVER_H
#define SOLVER_H

/*
 * solver.h
 * --------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This module reads the state of the cube and determines the sequence of algorithms to solve it.
 */

#include "commutators.h"
#include "cube.h"
#include "algorithms.h"

void solve_init(void);
char *trace_edges(void);
char *trace_corners(void);

#endif
