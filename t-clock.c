#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __linux__
#include <unistd.h>
#endif

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

#define N_FLAGS 2
char possible_flags[N_FLAGS][9] = {{"-allSeg\0"}, {"-useDots\0"}};

bool allSecSegments = false;
char pixel_shape[5] = "█\0";

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

#define DIAMETER 41
#define N_SEGMENTS 32
#define CLOCKFACE_OFFSET 1
#define N_CLOCKFACE_LINES 11

int CIRCLE_COORD[N_SEGMENTS][2] = {
    {0, 12},  {0, 14},  {0, 16}, {0, 18}, {1, 20}, {2, 22},  {3, 23},  {4, 24},
    {5, 24},  {6, 24},  {7, 23}, {8, 22}, {9, 20}, {10, 18}, {10, 16}, {10, 14},
    {10, 12}, {10, 10}, {10, 8}, {10, 6}, {9, 4},  {8, 2},   {7, 1},   {6, 0},
    {5, 0},   {4, 0},   {3, 1},  {2, 2},  {1, 4},  {0, 6},   {0, 8},   {0, 10}};

#else

#define DIAMETER 30
#define N_SEGMENTS 22
#define CLOCKFACE_OFFSET 0
#define N_CLOCKFACE_LINES 9

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
    for(int i = -CLOCKFACE_OFFSET; i < DISPLAY_HIGHT_L + CLOCKFACE_OFFSET;
        ++i) {
        for(int k = 0; k < n_targets; ++k) {
            uint16_t* k_target = PATTERNS[indices[k]];
            mask = 1 << (16 - 1);
            for(int j = 0; j < DISPLAY_WIDTH_L; ++j) {
                if(i >= 0 && i < DISPLAY_HIGHT_L && mask & k_target[i]) {
                    printf(GRN_F "%s" RESET, pixel_shape);
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
            bool to_print =
                circle_index == -1
                    ? false
                    : isPointOnClockFace(j, i + CLOCKFACE_OFFSET, circle_index);
            if(to_print) {
                printf(BLU_F "●" RESET);
            } else if(isPointOnClockFace(j, i + CLOCKFACE_OFFSET,
                                         N_SEGMENTS - 1) &&
                      allSecSegments) {
                printf("●");
            } else {
                printf(" ");
            }
        }

        printf("\n");
    }
}

int main(int argc, char** argv) {
    printf("\x1b[?25l \n");

#ifdef __linux__
    signal(SIGINT, handle_int);
#endif

    for(int i = 1; i < argc; ++i) {
        if(strcmp("-allSeg", argv[i]) == 0) {
            allSecSegments = true;
        } else if(strcmp("-useDots", argv[i]) == 0) {
            strcpy((char*)&pixel_shape, "●\0");
        } else {
            printf("Flag not recognized. Only following flags are allowed:\n");
            for(int j = 0; j < N_FLAGS; ++j) {
                printf("%s\n", possible_flags[j]);
            }
            printf("\x1b[?25h\n");
            return 0;
        }
    }

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
        printf("\r\x1b[%dA", DISPLAY_HIGHT_L + 2 * CLOCKFACE_OFFSET);

        sleep(1);
    }

    printf("\r\x1b[%dB", DISPLAY_HIGHT_L + 2 * CLOCKFACE_OFFSET);
    printf("\x1b[?25h\n");
    fflush(stdout);

    return 0;
}