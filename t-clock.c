#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RED_F "\x1B[31m"
#define GRN_F "\x1B[32m"
#define BLU_F "\x1B[34m"
#define RED_B "\x1B[41m"
#define GRN_B "\x1B[42m"
#define BLU_B "\x1B[44m"
#define RESET "\x1B[0m"

static volatile int running = 1;

const int DISPLAY_WIDTH_L = 11;
const int DISPLAY_HIGHT_L = 9;

bool allSecSegments = false;

#define ZERO \
    {0x1F00, 0x2080, 0x2080, 0x2080, 0x2080, 0x2080, 0x2080, 0x2080, 0x1F00}
#define ONE {0x400, 0xC00, 0x1400, 0x2400, 0x400, 0x400, 0x400, 0x400, 0x3F80}
#define TWO {0x1F00, 0x80, 0x80, 0x80, 0x1F00, 0x2000, 0x2000, 0x2000, 0x1F00}
#define THREE {0x1F00, 0x80, 0x80, 0x80, 0x1F00, 0x80, 0x80, 0x80, 0x1F00}
#define FOUR {0x2080, 0x2080, 0x2080, 0x2080, 0x1F00, 0x80, 0x80, 0x80, 0x80}
#define FIVE {0x3F80, 0x2000, 0x2000, 0x2000, 0x3F00, 0x80, 0x80, 0x80, 0x3F00}
#define SIX \
    {0x200, 0x400, 0x800, 0x1000, 0x1F00, 0x2080, 0x2080, 0x2080, 0x1F00}
#define SEVEN {0x3F80, 0x80, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000}
#define EIGHT \
    {0x1F00, 0x2080, 0x2080, 0x2080, 0x1F00, 0x2080, 0x2080, 0x2080, 0x1F00}
#define NINE \
    {0x1F00, 0x2080, 0x2080, 0x2080, 0x1F00, 0x200, 0x400, 0x800, 0x1000}
#define COLUMN {0x0, 0x0, 0xE00, 0xE00, 0x0, 0xE00, 0xE00, 0x0, 0x0}

uint16_t PATTERNS[][9] = {ZERO, ONE,   TWO,   THREE, FOUR,  FIVE,
                          SIX,  SEVEN, EIGHT, NINE,  COLUMN};
#ifdef BIGCIRCLE
#define DIAMETER 39
#define LINE0 "      ● ● ● ● ● ● ●     \0"
#define LINE1 "    ●               ●            \0"
#define LINE2 "  ●                   ●          \0"
#define LINE3 " ●                     ●         \0"
#define LINE4 "●                       ●        \0"
#define LINE5 "●                       ●        \0"
#define LINE6 "●                       ●        \0"
#define LINE7 " ●                     ●         \0"
#define LINE8 "  ●                   ●          \0"
#define LINE9 "    ●               ●            \0"
#define LINEA "      ● ● ● ● ● ● ●     \0"

char CIRCLE[11][DIAMETER] = {LINE0, LINE1, LINE2, LINE3, LINE4, LINE5,
                             LINE6, LINE7, LINE8, LINE9, LINEA};
#else
#define DIAMETER 30
#define N_SEGMENTS 22
#define LINE0 "      ● ●  ● ●      \0"
#define LINE1 "   ●            ●   \0"
#define LINE2 " ●                ● \0"
#define LINE3 "●                  ●\0"
#define LINE4 "●                  ●\0"
#define LINE5 "●                  ●\0"
#define LINE6 " ●                ● \0"
#define LINE7 "   ●            ●   \0"
#define LINE8 "      ● ●  ● ●      \0"

char CIRCLE[9][DIAMETER] = {LINE0, LINE1, LINE2, LINE3, LINE4,
                            LINE5, LINE6, LINE7, LINE8};

int CIRCLE_COORD[N_SEGMENTS][2] = {
    {0, 11}, {0, 13}, {1, 16}, {2, 18}, {3, 19}, {4, 19}, {5, 19}, {6, 18},
    {7, 16}, {8, 13}, {8, 11}, {8, 8},  {8, 6},  {7, 3},  {6, 1},  {5, 0},
    {4, 0},  {3, 0},  {2, 1},  {1, 3},  {0, 6},  {0, 8}};

#endif

time_t epoch_time;
struct tm struct_time;

void handle_int() {
    running = 0;
}

bool isPointOnClockFace(int x, int y, int max_Clock_Index) {
    bool is_On_Face = false;
    for(int i = 0; i <= max_Clock_Index && !is_On_Face; ++i) {
        int* current_ClockFace_Point = CIRCLE_COORD[i];
        is_On_Face =
            current_ClockFace_Point[1] == x && current_ClockFace_Point[0] == y;
    }

    return is_On_Face;
}

void render(uint16_t* indices, int n_targets, int circle_index) {
    uint16_t mask = 0;
    for(int i = 0; i < DISPLAY_HIGHT_L; ++i) {
        for(int k = 0; k < n_targets; ++k) {
            uint16_t* k_target = PATTERNS[indices[k]];
            mask = 1 << (16 - 1);
            for(int j = 0; j < DISPLAY_WIDTH_L; ++j) {
                if(mask & k_target[i]) {
                    printf(GRN_F "█" RESET);
                    fflush(stdout);
                } else {
                    printf(" ");
                    fflush(stdout);
                }
                mask = mask >> 1;
            }
            mask = 1 << (16 - 1);
        }

        printf("  ");
        for(int j = 0; j < DIAMETER; ++j) {
            bool to_print = circle_index == -1
                                ? false
                                : isPointOnClockFace(j, i, circle_index);
            if(to_print) {
                printf(BLU_F "●" RESET);
            } else if(isPointOnClockFace(j, i, N_SEGMENTS - 1) &&
                      allSecSegments) {
                printf("●");
            } else {
                printf(" ");
            }
        }

        printf("\n");
    }
}

int main() {
    printf("\x1b[?25l \n");

    signal(SIGINT, handle_int);

    uint16_t target_idxs[8];
    int circle_index = -1;
    while(running) {
        epoch_time = time(NULL);
        struct_time = *localtime(&epoch_time);

        target_idxs[0] = struct_time.tm_hour / 10;
        target_idxs[1] = struct_time.tm_hour % 10;
        target_idxs[2] = 10;
        target_idxs[3] = struct_time.tm_min / 10;
        target_idxs[4] = struct_time.tm_min % 10;
        target_idxs[5] = 10;
        target_idxs[6] = struct_time.tm_sec / 10;
        target_idxs[7] = struct_time.tm_sec % 10;

        circle_index = struct_time.tm_sec == 0
                           ? -1
                           : (float)struct_time.tm_sec / 60.0 * N_SEGMENTS;

        render((uint16_t*)&target_idxs, 8, circle_index);
#ifdef INFO
        printf("\rCurrent System Time: %2d:%2d:%2d", struct_time.tm_hour,
               struct_time.tm_min, struct_time.tm_sec);
        fflush(stdout);
#endif
        printf("\r\x1b[%dA", DISPLAY_HIGHT_L);

        struct timespec req = {0, 400000000};
        nanosleep(&req, NULL);
    }

    printf("\r\x1b[%dB", DISPLAY_HIGHT_L);
    printf("\x1b[?25h\n");
    fflush(stdout);

    return 0;
}