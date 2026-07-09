#include "../include/config.h"

#include <stdlib.h>
#include <stdio.h>

int emulator_config_initialise_default(struct emulator_config *const ec) { 
    if (ec == NULL) {
        printf("NULL");
        return ERR_CONFIG_NULL_PTR;
    }
    
    ec->beep_frequency = DEFAULT_BEEP_FREQUENCY;
    ec->instr_per_second = DEFAULT_INSTR_PER_SECOND;
    ec->frames_per_second = DEFAULT_FRAMES_PER_SECOND;
    ec->timer_decrease_rate = DEFAULT_TIMER_DECREASE_RATE;
    ec->window_width = DEFAULT_WINDOW_WIDTH;
    ec->window_height = DEFAULT_WINDOW_HEIGHT;
    ec->beep_amplitude = DEFAULT_BEEP_AMPLITUDE;
    ec->rom_path = "INVADERS.ch8";
    return CONFIG_INIT_SUCCESS;
}

int emulator_config_initialise_from_args(struct emulator_config *const ec, int argc, char **argv) {
    return emulator_config_initialise_default(ec);
}