#include "assert.h"
#include "cube.h"
#include "algorithms.h"
#include "printf.h"
#include "strings.h"
#include "uart.h"
#include "keyboard.h"
#include "ps2.h"
#include "motors.h"
#include "interrupts.h"
#include "calibrate.h"
#include "commutators.h"
#include "solver.h"
#include "strings.h"
#include "malloc.h"
#include <stdarg.h>

#define ESC_SCANCODE 0x76

static void test_cube(void) {
    while (1) {
        char c = keyboard_read_next();

        if (c == ps2_keys[ESC_SCANCODE].ch) break;
        switch (c) {
            case 'U':
                u_move(1);
                motor_move(WHITE, 1);
                break;
            case 'D':
                d_move(1);
                motor_move(YELLOW, 1);
                break;
            case 'R':
                r_move(1);
                motor_move(RED, 1);
                break;
            case 'L':
                l_move(1);
                motor_move(ORANGE, 1);
                break;
            case 'F':
                f_move(1);
                motor_move(GREEN, 1);
                break;
            case 'B':
                b_move(1);
                motor_move(BLUE, 1);
                break;
            case 'u':
                u_move(3);
                motor_move(WHITE, 3);
                break;
            case 'd':
                d_move(3);
                motor_move(YELLOW, 3);
                break;
            case 'r':
                r_move(3);
                motor_move(RED, 3);
                break;
            case 'l':
                l_move(3);
                motor_move(ORANGE, 3);
                break;
            case 'f':
                f_move(3);
                motor_move(GREEN, 3);
                break;
            case 'b':
                b_move(3);
                motor_move(BLUE, 3);
                break;
            case '7':
                u_move(2);
                motor_move(WHITE, 2);
                break;
            case '8':
                d_move(2);
                motor_move(YELLOW, 2);
                break;
            case '9':
                r_move(2);
                motor_move(RED, 2);
                break;
            case '0':
                l_move(2);
                motor_move(ORANGE, 2);
                break;
            case '-':
                f_move(2);
                motor_move(GREEN, 2);
                break;
            case '=':
                b_move(2);
                motor_move(BLUE, 2);
                break;
            case '1':
                calibrate_motor(WHITE, 1);
                break;
            case '2':
                calibrate_motor(YELLOW, 1);
                break;
            case '3':
                calibrate_motor(RED, 1);
                break;
            case '4':
                calibrate_motor(ORANGE, 1);
                break;
            case '5':
                calibrate_motor(GREEN, 1);
                break;
            case '6':
                calibrate_motor(BLUE, 1);
                break;
            case '!':
                calibrate_motor(WHITE, 0);
                break;
            case '@':
                calibrate_motor(YELLOW, 0);
                break;
            case '#':
                calibrate_motor(RED, 0);
                break;
            case '$':
                calibrate_motor(ORANGE, 0);
                break;
            case '%':
                calibrate_motor(GREEN, 0);
                break;
            case '^':
                calibrate_motor(BLUE, 0);
                break;
        }
        printf("%c\n", c);
        print_cube_state();
    }
}

static void test_inverter(void) {
    assert(strcmp(invert_alg("R"), "R' ") == 0);
    assert(strcmp(invert_alg("L U B' R2 D'"), "D R2 B U' L' ") == 0);
    assert(strcmp(invert_alg("R' f R2 U'"), "U R2 f' R ") == 0);
}

static void test_alg_parser(void) {
    printf("parse %s: %s\n", "R U R' U'", parse_alg("R U R' U'"));
    printf("parse %s: %s\n", "M U2 M' U2", parse_alg("M U2 M' U2"));
    printf("parse %s: %s\n", "f z D S' E l'", parse_alg("f z D S' E l'"));
}

static void test_clear_rotations(void) {
    printf("clear rotations %s: %s\n", "R U R' U'", clear_rotations("R U R' U'"));
    printf("clear rotations %s: %s\n", "y y'", clear_rotations("y y'"));
    printf("clear rotations %s: %s\n", "x2 D x2", clear_rotations("x2 D x2"));
    printf("clear rotations %s: %s\n", "x2 z2 y2 U y2 z2 x2", clear_rotations("x2 z2 y2 U y2 z2 x2"));
    printf("clear rotations %s: %s\n", "y R D x' F L x U' B y'", clear_rotations("y R D x' F L x U' B y'"));
    printf("clear rotations %s: %s\n", "x' R L' U2 L R' x U2", clear_rotations("x' R L' U2 L R' x U2"));
}

static void test_comm_parser(void) {
    printf("parse %s (inverted): %s\n", "R U R' U' R' F R2 U' R' U' R U R' F'", parse_comm("R U R' U' R' F R2 U' R' U' R U R' F'", true)); // T perm
    printf("parse %s (inverted): %s\n", "[R, U' L' U]", parse_comm("[R, U' L' U]", true)); // RQ (corners)
    printf("parse %s: %s\n", "[M, U2]", parse_comm("[M, U2]", false)); // AW (edges)
    printf("parse %s: %s\n", "[U' :[R2, U' L2 U]]", parse_comm("[U' :[R2, U' L2 U]]", false)); // UV (corners)
    printf("parse %s (inverted): %s\n", "[x' z' U' :[R, U' M U]]", parse_comm("[x' z' U' :[R, U' M U]]", true)); // XM (edges) 
    printf("parse %s: %s\n", "[M' :[R E' R', U]]", parse_comm("[M' :[R E' R', U]]", false)); // RM (edges)
    printf("parse %s (inverted): %s\n", "[L' U' L :[S', L2]]", parse_comm("[L' U' L :[S', L2]]", true)); // DX (edges)
    printf("parse %s: %s\n", "[y2 z :[R, U' M U]]", parse_comm("[y2 z :[R, U' M U]]", false)); 
    printf("parse %s (inverted): %s\n", "[x2 y' U' :[R, U' M U]]", parse_comm("[x2 y' U' :[R, U' M U]]", true));
    printf("parse %s (inverted): %s\n", "[x' :[U' M2 U, R]]", parse_comm("[x' :[U' M2 U, R]]", true));
    printf("parse %s: %s\n", "[L2 U :[U2, S]]", parse_comm(edge_comm_list[A][D], false));
    printf("parse %s: %s\n", "[x' z :[L', U M2 U']]", parse_comm("[x' z :[L', U M2 U']]", false)); 
}

static void test_comm_list(void) {
    printf("Testing edge comm list\n");
    printf("AB: %s, should be [R2 U' :[U2, S']]\n", edge_comm_list[A][B]); 
    printf("HM: %s, should be [R F :[R2, E']]\n", edge_comm_list[H][M]);
    printf("PA: %s, should be empty\n", edge_comm_list[P][A]);
    printf("Testing corner comm list\n");
    printf("AB: %s, should be [R' :[F, R' B2 R]]\n", corner_comm_list[A][B]); 
    printf("HM: %s, should be empty\n", corner_comm_list[H][M]);
    printf("PX: %s, should be [R :[R D' R', U']]\n", corner_comm_list[P][X]);
    printf("Testing parity alg list\n");
    printf("W: %s, should be U R2 U' R U R' F' R U R' U' R' F R2 U' R U'\n", parity_alg_list[W]);
}

static void test_clear_duplicates(void) {
    printf("clear duplicates L L' R R2 F' F2 B' B' D2 D2 U U x2 x' z2 z: %s\n", clear_duplicates("L L' R R2 F' F2 B' B' D2 D2 U U x2 x' z2 z"));
    printf("clear duplicates R L R' L': %s\n", clear_duplicates("R L R' L'"));
    printf("clear duplicates R L R2 D' U D': %s\n", clear_duplicates("R L R2 D' U D'"));
    printf("clear duplicates R B B' R': %s\n", clear_duplicates("R B B' R'"));
    printf("clear duplicates D F L' B2 B2 L R U': %s\n", clear_duplicates("D F L' B2 B2 L R U'"));
    printf("clear duplicates D F F2 L' B2 B2 L R U': %s\n", clear_duplicates("D F F2 L' B2 B2 L R U'"));
    printf("clear duplicates B' U U2: %s\n", clear_duplicates("B' U U2"));
    printf("clear duplicates B U U2: %s\n", clear_duplicates("B U U2"));
}

static void perform_solve(char *solve) { 
    while (*solve) {
        char move = *solve;
        if (move == ' ' || move == 0x27 || move == '2') {
            solve++;
            continue;
        }
        char modifier = *(solve+1);
        switch (move) {
            case 'U':
                switch (modifier) {
                    case ' ':
                        u_move(1);
                        motor_move(WHITE, 1);
                        break;
                    case 0x27:
                        u_move(3);
                        motor_move(WHITE, 3);
                        break;
                    case '2':
                        u_move(2);
                        motor_move(WHITE, 2);
                        break;
                }
                break;
            case 'D':
                switch (modifier) {
                    case ' ':
                        d_move(1);
                        motor_move(YELLOW, 1);
                        break;
                    case 0x27:
                        d_move(3);
                        motor_move(YELLOW, 3);
                        break;
                    case '2':
                        d_move(2);
                        motor_move(YELLOW, 2);
                        break;
                }
                break;
            case 'R':
                switch (modifier) {
                    case ' ':
                        r_move(1);
                        motor_move(RED, 1);
                        break;
                    case 0x27:
                        r_move(3);
                        motor_move(RED, 3);
                        break;
                    case '2':
                        r_move(2);
                        motor_move(RED, 2);
                        break;
                }
                break;
            case 'L':
                switch (modifier) {
                    case ' ':
                        l_move(1);
                        motor_move(ORANGE, 1);
                        break;
                    case 0x27:
                        l_move(3);
                        motor_move(ORANGE, 3);
                        break;
                    case '2':
                        l_move(2);
                        motor_move(ORANGE, 2);
                        break;
                }
                break;
            case 'F':
                switch (modifier) {
                    case ' ':
                        f_move(1);
                        motor_move(GREEN, 1);
                        break;
                    case 0x27:
                        f_move(3);
                        motor_move(GREEN, 3);
                        break;
                    case '2':
                        f_move(2);
                        motor_move(GREEN, 2);
                        break;
                }
                break;
            case 'B':
                switch (modifier) {
                    case ' ':
                        b_move(1);
                        motor_move(BLUE, 1);
                        break;
                    case 0x27:
                        b_move(3);
                        motor_move(BLUE, 3);
                        break;
                    case '2':
                        b_move(2);
                        motor_move(BLUE, 2);
                        break;
                }
                break;
        }
        solve++;
    }
}

static void test_corner_trace(void) {
    cube_init();
    solve_init();
    f_move(3);
    u_move(3);
    r_move(2);
    b_move(2);
    u_move(1);
    f_move(2);
    u_move(3);
    r_move(2);
    f_move(2);
    d_move(1);
    r_move(2);
    b_move(2);
    l_move(3);
    f_move(1);
    d_move(2);
    f_move(3);
    l_move(2);
    d_move(1);
    l_move(3);
    //F' U' R2 B2 U F2 U' R2 F2 D R2 B2 L' F D2 F' L2 D L'
    char *corner_trace = trace_corners();
    printf("%s\n", corner_trace);
    /*
    cube_init();
    solve_init();
    f_move(3);
    u_move(1);
    f_move(2);
    d_move(3);
    u_move(2);
    b_move(2);
    u_move(1);
    r_move(2);
    d_move(1);
    u_move(1);
    b_move(3);
    r_move(1);
    u_move(2);
    b_move(3);
    u_move(1);
    r_move(2);
    f_move(1);
    l_move(3);
    d_move(3);
    l_move(2);
    u_move(3);

    b_move(1);
    u_move(2);
    r_move(3);
    u_move(1);
    char *corner_trace_1 = trace_corners();
    printf("%s\n", corner_trace_1);*/
}

static void test_edge_trace(void) {
    /*
    cube_init();
    solve_init();
    f_move(3);
    u_move(3);
    r_move(2);
    b_move(2);
    u_move(1);
    f_move(2);
    u_move(3);
    r_move(2);
    f_move(2);
    d_move(1);
    r_move(2);
    b_move(2);
    l_move(3);
    f_move(1);
    d_move(2);
    f_move(3);
    l_move(2);
    d_move(1);
    l_move(3);
    //F' U' R2 B2 U F2 U' R2 F2 D R2 B2 L' F D2 F' L2 D L'
    char *edge_trace = trace_egdes();
    printf("%s\n", edge_trace);
    */
    cube_init();
    solve_init();
    f_move(3);
    u_move(1);
    f_move(2);
    d_move(3);
    u_move(2);
    b_move(2);
    u_move(1);
    r_move(2);
    d_move(1);
    u_move(1);
    b_move(3);
    r_move(1);
    u_move(2);
    b_move(3);
    u_move(1);
    r_move(2);
    f_move(1);
    l_move(3);
    d_move(3);
    l_move(2);
    u_move(3);
    
    r_move(1);
    b_move(1);
    r_move(3);
    print_cube_state();
    trace_corners();
    char *edge_trace_1 = trace_edges();
    printf("%s\n", edge_trace_1); //BWLNAUXDOPHE BWLNAUXMSFTQKGDOPHE BWLNAUXDOPHE
}

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

static void test_check_comms(void) {
    printf("Starting comm edges\n");
    for (int i = A; i <= W; i++) {
        for (int j = i; j <= X; j++) { 
            if (i == F && j == L) continue;
            if (strcmp(edge_comm_list[i][j], "") == 0) continue;
            char *pair = malloc(3);
            pair[0] = i + 65;
            pair[1] = j + 65;
            pair[2] = '\0';
            cube_init();
            solve_init();
            //printf("Performing comm %s\n", edge_comm_list[i][j]);
            char *parsed = parse_comm(edge_comm_list[i][j], false);
            //printf("After parsing: %s\n", parsed);
            perform_solve(parsed);         
            char *edge_trace = trace_edges();
            printf("%s alg gets trace %s\n", pair, edge_trace);
            clear_memory(3, pair, parsed, edge_trace);
        }
    }
    printf("Starting comm corners\n");
    for (int i = A; i <= W; i++) {
        for (int j = i; j <= X; j++) {
            if (i == L && j == R) continue;
            if (strcmp(corner_comm_list[i][j], "") == 0) continue;
            char *pair = malloc(3);
            pair[0] = i + 65;
            pair[1] = j + 65;
            pair[2] = '\0';
            cube_init();
            solve_init();
            //printf("Performing comm %s\n", corner_comm_list[i][j]);
            char *parsed = parse_comm(corner_comm_list[i][j], false);
            //printf("After parsing: %s\n", parsed);
            perform_solve(parsed);
            char *corner_trace = trace_corners();
            printf("%s alg gets trace %s\n", pair, corner_trace);
            clear_memory(3, pair, parsed, corner_trace);
        }
   }
}

void test_are_opposites(void) {
    printf("Are opposites 14 15: %d\n", are_opposites(14, 15));
    printf("Are opposites 0 3: %d\n", are_opposites(0, 3));
    printf("Are opposites 15 12: %d\n", are_opposites(15, 12));
}

void main(void) {
    uart_init();
    cube_init();
    interrupts_init(); 
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    interrupts_global_enable();
    motor_init();

    printf("Testing cube. Press esc to escape.\n");
    print_cube_state();
    test_cube();
    printf("Testing inverter\n");
    test_inverter();
    printf("Testing alg parser\n");
    test_alg_parser();
    printf("Testing clear rotations\n");
    test_clear_rotations();
    printf("Testing comm parser\n");
    test_comm_parser();
    printf("Testing comm list\n");
    test_comm_list();
    printf("Testing clear duplicates\n");
    test_clear_duplicates();
    printf("Testing corner trace\n"); 
    test_corner_trace();
    printf("Testing edge trace\n"); 
    test_edge_trace();
    printf("Testing are opposites\n");
    test_are_opposites();

    test_check_comms();
    uart_putchar(EOT);
}
