#include <signal.h>
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

time_t epoch_time;
struct tm struct_time;

void handle_int() {
    running = 0;
}

void render(uint16_t* indices, int n_targets) {
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
        printf("\n");
    }
}

int main() {
    printf("\x1b[?25l\n");

    signal(SIGINT, handle_int);

    uint16_t target_idxs[8];
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

        render((uint16_t*)&target_idxs, 8);
#ifdef INFO
        printf("\rCurrent System Time: %2d:%2d:%2d", struct_time.tm_hour,
               struct_time.tm_min, struct_time.tm_sec);
        fflush(stdout);
#endif
        printf("\r\x1b[%dA", DISPLAY_HIGHT_L);

        struct timespec req = {0, 9000000};
        nanosleep(&req, NULL);
    }

    printf("\r\x1b[%dB", DISPLAY_HIGHT_L);
    printf("\x1b[?25h\n");
    fflush(stdout);

    return 0;
}