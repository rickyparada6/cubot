/*
 * commutators.c
 * -------------
 * Author: Ethan Li
 * Author: Ricky Parada
 *
 * This module defines the edge and corner commutators as well as parity algorithms.
 */

#include "commutators.h"

const char *edge_comm_list[23][24] = {
    // A
    {"", "[R2 U' :[U2, S']]", "", "[L2 U :[U2, S]]", "[S' U :[U2, S]]", "[U' :[L' S L, U2]]", "[U :[L E L', U2]]", "[U' :[R E' R', U2]]", "", "[R' U' :[U2, S']]", "[U' M U :[M, U2]]", "[L U :[U2, S]]", "[S U :[S', U2]]", "[U :[L' E L, U2]]", "[U' :[R' E' R, U2]]", "[U :[R S' R', U2]]", "U' S R' F' R S' R' F R U' M' U2 M", "[L' U :[U2, S]]", "[U' M' U :[U2, M']]", "[R U' :[U2, S']]", "[U2, M']", "[U' :[U2, S']]", "[M, U2]", "[U :[U2, S]]"},
    // B
    {"", "", "", "[F2 U :[U2, M']]", "[L' U' L, S]", "[U', L' S L]", "[U :[L E L', U]]", "[U', L S L']", "", "[U :[L' E2 L, U]]", "[x :[U' R' U, M]]", "[U', R E2 R']", "R' E2 R2 E' R' U' R E R2 E2 R U", "[U :[R' S' R, U]]", "[U', R E R']", "[U :[R S' R', U]]", "[U :[R' F R, S]]", "[U', R' E2 R]", "[D' :[U', R' E' R]]", "[U :[L E2 L', U]]", "R2 f2 R2 U' R2 f2 R2 U'", "U R' U' R U R U R U' R' U2", "[M2, R U' R' U]", "U' L' U' L U L U L U' L'"},
    // C
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
    // D
    {"", "", "", "", "L E2 L2 E L U L' E' L2 E2 L' U'", "[U' :[L' S L, U']]", "[R U R', S]", "[U' :[L S L', U']]", "", "[U, R' S2 R]", "[x :[U L U', M]]", "U2 L U L U L' U' L' U' L' U'", "[R U R', S']", "[U, R' S' R]", "[U' :[R E R', U']]", "[U, R S' R']", "[M, L' U L U']", "L' U' L' U' L' U L U L U", "[D :[U, L E L']]", "R U R U R' U' R' U' R' U", "R2 f2 R2 U R2 f2 R2 U", "U R U R' U' R' U' R' U R", "D' U R U R' U' R' U' R' U R D", "U' L U L' U' L' U' L' U L U2"},
    // E
    {"", "", "", "", "", "[r' :[U M2 U', L']]", "[L2, D M D']", "[r' :[U M2 U', L]]", "", "[F R U :[U2, M']]", "[x :[M', U' L U]]", "[F' :[L', U M2 U']]", "[R F R' :[R2, S]]", "[r' U :[M', U2]]", "[x' z' :[M2, U R U']]", "[x' z' :[U' M2 U, R]]", "[U' :[S, R' F R]]", "[U' F' L :[S', L2]]", "[x U' :[U' M' U, L]]", "[R' :[S, L U' L']]", "[x L' D :[U2, M']]", "[L :[F', L S L']]", "[x :[M2, U' L U]]", "[L' :[E, L' U L]]"},
    // F
    {"", "", "", "", "", "", "[L, U S' U']", "[x' :[L2, U M2 U']]", "", "[z' :[M', U L' U']]", "[M' :[M', U' L' U]]", "z' x L E2 L2 E L U L' E' L2 E2 L' U' x' z", "[x' z :[L', U M2 U']]", "[E :[R U' R', E2]]", "[y' :[U' M U, R]]", "[x M2 U :[M', U2]]", "[U' L' U, M']", "[F', L S' L']", "x U M' U M U M' U M x'", "[F2 :[F', R S' R']]", "[x L2 U :[S', L2]]", "[y' :[U M2 U', R]]", "[x' :[U' L2 U, M2]]", "[y :[U' M2 U, L]]"},
    // G
    {"", "", "", "", "", "", "", "[U S' U', L']", "", "[D :[U' R U, M']]", "[x :[M', U' L' U]]", "[L u L' :[E, L2]]", "[x' z :[U' L' U, M2]]", "[F R :[R2, S']]", "[R' F R :[R2, S']]", "[y :[L', U M U']]", "[L' :[U' L' U, M']]", "[D :[U L U', M']]", "[x U' :[U' M' U, L']]", "[R :[L' F' L, S]]", "[y' :[R2, U M' U']]", "[x :[L' U' L, E']]", "[x :[M2, U' L' U]]", "S L F' L' U M' U2 M U S' L F L'"},
    // H
    {"", "", "", "", "", "", "", "", "", "[z' U :[U M U', L']]", "[x :[M', U' L2 U]]", "[E, L' U L]", "[R F :[R2, E']]", "[x' U :[M', U2]]", "[F' L :[L2, S]]", "[E' :[E2, L' U L]]", "[U' L U, M']", "B' U' S R' F' R S' R' F R U' M' U2 M B", "[x U' :[U' M' U, L2]]", "[E', R' U' R]", "[B' :[U2, M']]", "[y' L' U :[M, U2]]", "[D' L' :[L' F' L, S]]", "[y R' U :[U2, M]]"},
    // I
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
    // J
    {"", "", "", "", "", "", "", "", "", "", "[M', U' R U]", "[L' :[L' E2 L, U]]", "[F :[U' M2 U, R]]", "[R U' R', E']", "[D' :[M', U' R U]]", "E2 R2 E' R' U' R E R2 E2 R U R'", "[M, U' R U]", "[R U' R', E2]", "[U' :[U' M U, R]]", "[U R' :[S, R2]]", "R' f2 R2 U' R2 f2 R2 U' R'", "[R', D M2 D']", "[U R U: [U2, M]]", "[U L :[E', L2]]"},
    // K
    {"", "", "", "", "", "", "", "", "", "", "", "[U L' U', M']", "[M, D R2 D']", "[x :[U R2 U', M']]", "[x :[U R U', M']]", "[M' :[U R U', M']]", "[U' :[S, R' F' R]]", "[U L U', M']", "[z' x :[U' M' U, R2]]", "[U' R' U, M']", "x U' S R' F' R S' R' F R U' M' U2 M x'", "[U' R2 U, M']", "[D :[R F R', S']]", "[U L2 U', M']"},
    // L
    {"", "", "", "", "", "", "", "", "", "", "", "", "[F' L' U :[U2, M']]", "[L' :[U, L' E L]]", "[D' :[M', U L' U']]", "[L' U L, E']", "[M, U L' U']", "[U' L' :[L2, S']]", "[U :[U M U', L']]", "[E2, R' U' R]", "[U' L' U :[U2, M']]", "[U' R' :[E, R2]]", "[M2, U L' U']", "L2 U' L' U' L' U' L U L U L'"},
    // M
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "[S' :[U, L' E L]]", "[F' R' :[R2, E]]", "[S' :[U, L E' L']]", "[R U R' U', M']", "[L :[S', R U R']]", "[x U :[U M' U', R']]", "[U F R :[R2, S]]", "[R F :[R2, E]]", "[R' F R, S']", "[x :[M2, U R' U']]", "[x' z' U' :[R, U' M U]]"},
    // N
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "[R, D' M D]", "[x' :[U' M2 U, R2]]", "[U R' U', M']", "[E, L U L']", "[D' :[R, D' M D]]", "R2 E2 R2 E' R' U' R E R2 E2 R U R", "[F :[R2, E]]", "[y R U :[M', U2]]", "[x :[M2, U R2 U']]", "[y R U :[U2, M]]"},
    // O
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[D' M D, R']", "[U R2 U', M']", "[D' :[U L U', M']]", "[U' :[R B R', S']]", "[S U L :[E, L2]]", "[R' F :[R2, E]]", "S' R' F R U' M' U2 M U' S R' F' R", "[x :[M2, U R U']]", "[R F R', S']"},
    // P
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U R U', M']", "[L :[L E' L', U]]", "[D' :[R', D' M D]]", "[E, R' U' R]", "[R2 F :[R2, E]]", "[y' :[U M2 U', R']]", "[x' :[U R2 U', M2]]", "[F, R' S' R]"},
    // Q
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U L U', M]", "[y :[U' M' U, R2]]", "[U' R' U, M]", "[y :[L2, U M' U']]", "[U' R2 U, M]", "[D' :[U' R2 U, M]]", "[U L2 U', M]"},
    // R
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U :[U M U', L]]", "[R U' R' :[R2, E']]", "[U' L U :[U2, M']]", "[U' R :[E', R2]]", "[M2, U L U']", "U' L' U' L' U L U L U L'"},
    // S
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U' :[R', U' M U]]", "[D' :[R F R', S']]", "[U' :[R2, U' M U]]", "B2 U' S R' F' R S' R' F R U' M' U2 M B2", "[U :[L2, U M U']]"},
    // T
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U R' U' :[U2, M']]", "[x' :[U M' U', R']]", "[M2, U' R' U]", "[U L :[L2, E]]"},
    // U
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "R2 U R2 f2 R2 U R2 f2", "[D2, M]", "L2 U' L2 f2 L2 U' L2 f2"},
    // V
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[M2, U' R2 U]", "[D :[M, D2]]"},
    // W
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U L2 U', M2]"}
};

const char *corner_comm_list[23][24] = {
    // A
    {"", "[R' :[F, R' B2 R]]", "", "[y L :[F', L B2 L']]", "[U2, R' D R D' R' D R]", "[F :[U2, R' D' R]]", "[R' D R, U2]", "[U2, L' D' L]", "[l' :[U2, R' D2 R]]", "", "[U2, L' D2 L]", "[U :[U2, R D2 R']]", "", "[R D R' :[R' D2 R, U2]]", "[U2, L' D L]", "[R' D' R, U2]", "[R' D' :[U2, R' D R]]", "[R' D R D' R' D R, U2]", "[R' D2 R, U2]", "[D' :[R' D' R, U2]]", "[R F' R' U' :[R D R', U2]]", "[R2 :[U, R2 D R2 D' R2]]", "[R D' R' :[R' D R, U2]]", "[z D' R' :[R' D R, U2]]"},
    // B
    {"", "", "", "[l D :[R' D' R, U]]", "[R :[U, R D R']]", "[R' :[R' D' R, U']]", "[R' D R, U]", "[U', R D' R']", "[R :[U2, R D R']]", "", "[D' :[R' D R, U]]", "[U', R D2 R']", "", "[R' D R D' R' D R, U]", "[U', R D R']", "[R' D' R, U]", "[U, R' D R D' R' D R]", "[R' :[R' D' R, U2]]", "[R' D2 R, U]", "[D :[U', R D' R']]", "[F2, R B R']", "[R2 D' R2 D R2, U]", "[U', R2 D R2 D' R2]", "[R' F' R, B2]"},
    // C 
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
    // D
    {"", "", "", "", "[U' R :[R D R', U2]]", "[R' D R D' R' D R, U']", "[R' D R, U']", "[U' :[R D' R', U']]", "[U', R' D R D' R' D R]", "", "[D' :[R' D R, U']]", "[D :[R' D' R, U']]", "", "[U' R' :[U2, R' D' R]]", "[U' :[R D R', U']]", "[R' D' R, U']", "[U' R :[R D R', U]]", "[U' R' :[U', R' D' R]]", "[R' D2 R, U']", "[U, L D2 L']", "[U' :[R B R', F2]]", "[U :[F2, U' L U L']]", "[R D' R' :[R' D R, U']]", "[R' D' R :[R U' R', D]]"},
    // E
    {"", "", "", "", "", "[L, U' R' U]", "[U R' :[U', R' D R]]", "[z U :[R' D' R, U]]", "[l' U' :[U2, R D' R']]", "", "[R U R' :[R' D' R, U']]", "[R U :[R' D R, U]]", "", "[R :[R2 D R2 D' R2, U]]", "[U' R :[R U' R', D']]", "[x :[R' D2 R, U]]", "R' F' r U R U' r' F", "[U2, R' D R D' R' D R]", "[U R' :[U', R' D2 R]]", "[R U :[R' D' R, U]]", "[R :[R D2 R', U]]", "[D2 R :[R D' R', U]]", "[D R :[R D' R', U]]", "[R :[R D' R', U]]"},
    // F
    {"", "", "", "", "", "", "[U' R' U, L']", "[z :[R' D' R, U2]]", "[R' D R D' R' D R, U']", "", "[R, U L U']", "[D R' U' :[D', R U R']]", "", "[F' U :[R' D R, U2]]", "[D' R :[F, R' U R U']]", "[R' U' :[D', R U R']]", "[R', U L U']", "[R B' R', F]", "[R B2 R', F]", "[F :[D', R U R']]", "[U' R' U :[R U' R', D]]", "[R' F2 :[R U' R', D']]", "[z :[D2, R U R']]", "[R2 D' :[D', R U' R']]"},
    // G
    {"", "", "", "", "", "", "", "[z' :[R U' R', D']]", "[D R' :[F2, R D' R' D]]", "", "[U :[D, R U' R']]", "R' D R U R' U' R U R' D' R U' R' U R U'", "", "[U R U :[R' D R, U2]]", "[U' :[D2, R' U' R]]", "[l :[D2, R U R']]", "[R' U :[R U' R', D]]", "[R' D :[R' U R, D]]", "[U :[L' U' L, D]]", "[R' D :[R' U' R, D]]", "U R' U' R U R' D R U' R' U R U' R' D' R", "[z R' D :[R U' R', D2]]", "[F' :[R U' R', D']]", "[F' :[R U' R', D2]]"},
    // H
    {"", "", "", "", "", "", "", "", "[l' :[U2, R' D R]]", "", "[U R U' :[R D' R', U']]", "[D, L' U L]", "", "[U R :[R D' R', U2]]", "[R D' :[R' U' R, D2]]", "[D2, R U R']", "[U' L U, R]", "[U2 R :[R D' R', U]]", "R U' R' U R D' R' U' R U R' U' R D R' U", "[D' :[D', R U R']]", "[D L' U :[F2, U' L U L']]", "[D2 R :[R D' R', U']]", "[R D' R' :[R' D R, U]]", "U' R D' R' U R U' R' U R D R' U' R U R'"},
    // I
    {"", "", "", "", "", "", "", "", "", "", "[R' :[R D' R' D, F2]]", "[R U :[R' D R, U2]]", "", "[R' :[U', R2 D' R2 D R2]]", "[l' U' :[R D R', U2]]", "[l' :[R' D' R, U2]]", "[R D :[R D' R', U2]]", "R f' r2 F D' F' D r2 U' f", "[D R' D :[F2, D' R D R']]", "[R U :[R' D' R, U2]]", "[L :[F2, L' D L D']]", "[D' L :[F2, L' D L D']]", "[D R :[R D' R', U2]]", "[R :[R D' R', U2]]"},
    // J
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
    // K
    {"", "", "", "", "", "", "", "", "", "", "", "[U L' U', R]", "", "[U' R' U :[R D R', U2]]", "[U :[R U' R', D']]", "R' D' R U R' U' R U R' D R U' R' U R U'", "[D' R' U :[R U' R', D]]", "[R :[U' L' U, R]]", "[U :[R U' R', D2]]", "[D R D :[R' U' R, D2]]", "[R :[R' U R U', F2]]", "U R' U' R U R' D' R U' R' U R U' R' D R", "[U R D' :[U', R' D R]]", "[z :[R U2 R', D]]"},
    // L
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "[F' U :[R' D' R, U2]]", "[R D :[D, R' U' R]]", "[D, R U R']", "[R', U L' U']", "L' F R F' L2 F R' F' L'", "[D' R D :[D2, R' U' R]]", "[D :[R U R', D2]]", "R' D R U R' U' R U R' D' R U' R' U R U'", "[R U' R' :[R' D R, U2]]", "[D R :[R D' R', U']]", "[z' U' :[R U R', D']]"},
    // M
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
    // N
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U R U' :[R' D' R, U2]]", "[F', R U R2 U' R']", "[R' D R D' R' D R, U]", "[U R :[U2, R D R']]", "[U' R' U :[U2, R D' R']]", "[U D R :[U2, R D' R']]", "[U' R' :[R' D R, U2]]", "[U' D' R' :[R' D R, U2]]", "[F' :[U2, R D R']]", "[U' D R' :[R' D R, U2]]"},
    // O
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[R D' :[R' D R, U]]", "[U' R U :[R' U' R, D]]", "[R' U :[R' D' R, U]]", "[U' :[R' U' R, D']]", "U R U' R' D' R U R' U' R U R' D R U' R'", "[U' R' :[R' D R, U]]", "[U D R :[D2, R U' R']]", "R U R' D' R U' R' U R U' R' D R U R' U'", "[R2 D2 :[R U R', D]]"},
    // P
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[R' U :[R U' R', D']]", "[R B' R', F']", "[l :[U2, R' D' R]]", "[R U R', D']", "[R :[R D2 R', U']]", "R' D' R U R' U' R U R' D R U' R' U R U'", "[D' R U' R' :[R' D R, U2]]", "[R :[R D' R', U']]"},
    // Q
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[R, U' L' U]", "[R' U :[D2, R U' R']]", "[R U :[D', R U R']]", "[R U D :[D2, R U R']]", "[U R' U' :[R U R', D']]", "[D R U' :[R' U R, D']]", "[R U' :[R' U R, D']]"},
    // R
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[R' D' :[R' U R, D2]]", "[D' R' :[R' U R, D]]", "[U' R' :[R' D R, U']]", "[U D R :[D', R U' R']]", "[U R :[D', R U' R']]", "[R :[U R U', L]]"},
    // S
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U' :[R', U' L U]]", "[x' :[D2, R U2 R']]", "[U D R :[D, R U' R']]", "[U R :[D2, R U' R']]", "R U' R' U R D' R' U' R U R' U' R D R' U"},
    // T
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[D' R :[R D' R', U']]", "[D' R D :[U, R' D' R]]", "U R U' R' D' R U R' U' R U R' D R U' R'", "[R U' D :[R' U R, D2]]"},
    // U
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[U' :[R2, U' L2 U]]", "[R2, U' R2 D R2 U]", "[F2, r D r']"},
    // V
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[R2 U R2 U' R2, D']", "[R2 :[D', R2 U' R2 U R2]]"},
    // W
    {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "[z :[R U2 R', D2]]"}
};

const char *parity_alg_list[] = {
    "U2 F R U' R' U' R U R' F' R U R' U' R' F R F' U2 ", // A
    "R U R' F' R U R' U' R' F R2 U' R' U' ", // B
    "", // C (buffer)
    "U L' U' L F L' U' L U L F' L2 U L ", // D
    "U R' U' R U R' F' R U R' U' R' F R2 U2 ", // E
    "U2 R' U R U2 R' L' U R U' L ", // F
    "D U R U' R U R' F' R U R' U' R' F R2 U' R2 U' D' ", // G
    "R D' U R' F' R U R' U' R' F R2 U' R' U' R D R' ", // H
    "U2 R' F R U' R' U' R U R' F' R U R' U' R' F R F' R U2 ", // I
    "", // J (buffer)
    "U R U' R U R' F' R U R' U' R' F R2 U' R2 U' ", // K
    "D U2 R' F R2 U' R' U' R U R' F' R U R' U D' ", // L
    "", // M (buffer)
    "U R' U R U' x' U L' U L U2 R U' R' U x ", // N
    "R D U R' F' R U R' U' R' F R2 U' R' U' R D' R' ", // O
    "U2 R' F R2 U' R' U' R U R' F' R U R' U ", // P
    "U R U' R' U2 L R U' R' U L' U ", // Q
    "U' L' U2 R U' R' U2 L R U' R' U2 ", // R
    "U' L U R U R' F' R U R' U' R' F R2 U' R' U2 L' U ", // S
    "D' U2 R' F R2 U' R' U' R U R' F' R U R' U D ", // T
    "U' x U2 R2 F R F' R U2 r' U L U ", // U
    "D U R2 U' R U R' F' R U R' U' R' F R2 U' R U' D' ", // V
    "U R2 U' R U R' F' R U R' U' R' F R2 U' R U' ", // W
    "D' U R2 U' R U R' F' R U R' U' R' F R2 U' R U' D " // X
};
