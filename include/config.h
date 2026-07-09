#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// Default config options
#define DEFAULT_BEEP_FREQUENCY 864

#define DEFAULT_INSTR_PER_SECOND 700
#define DEFAULT_FRAMES_PER_SECOND 60 
#define DEFAULT_TIMER_DECREASE_RATE 60

#define DEFAULT_WINDOW_WIDTH 1600
#define DEFAULT_WINDOW_HEIGHT 800

// Config limits
#define NON_ZERO_MIN 1 
#define MINIMUM_WINDOW_WIDTH 64
#define MINIMUM_WINDOW_HEIGHT 32

// Return Codes
#define CONFIG_INIT_SUCCESS 0
#define ERR_CONFIG_NULL_PTR -1
#define ERR_MISSING_ARGUMENT -2
#define ERR_INVALID_WINDOW_WIDTH -3
#define ERR_INVALID_WINDOW_HEIGHT -4
#define ERR_INVALID_FPS -5
#define ERR_INVALID_IPS -6
#define ERR_INVALID_TDR -7
#define ERR_UNKNOWN_FLAG -8

// Command Line Flags
#define FLAG_WINDOW_WIDTH "-w"
#define FLAG_WINDOW_HEIGHT "-h"
#define FLAG_FPS "--fps"
#define FLAG_IPS "--ips"
#define FLAG_TIMER_DECREASE_RATE "-t"

// Command Line Quirk Flags
#define FLAG_Q_ARITH_INSTR_OVERFLOW_RESET "--q-arith-instr-overflow-reset"
#define FLAG_Q_SHIFT_ONLY_VX "--q-shift-only-vx"
#define FLAG_Q_ADD_TO_INDEX_OVERFLOW "--q-add-to-index-overflow"
#define FLAG_Q_STORE_LOAD_INCREMENT_INDEX "--q-store-load-increment-index"

struct emulator_config {
    double beep_frequency;
    double instr_per_second;
    double frames_per_second;
    double timer_decrease_rate;
    unsigned window_width;
    unsigned window_height;
    int q_arith_instr_overflow_reset; 
    int q_shift_only_vx;
    int q_store_load_increment_index;
    int q_add_to_index_overflow;
    const char *rom_path;
};

int emulator_config_initialise_default(struct emulator_config *const ec); 
int emulator_config_initialise_from_args(struct emulator_config *const ec, int argc, char **argv);

#endif