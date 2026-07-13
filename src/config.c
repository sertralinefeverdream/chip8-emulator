#include "../include/config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

int emulator_config_initialise_default(struct emulator_config *const ec) { 
    if (ec == NULL) {
        printf("NULL");
        return ERR_CONFIG_NULL_PTR;
    }
   
    memset(ec, 0, sizeof(*ec));
    ec->beep_frequency = DEFAULT_BEEP_FREQUENCY;
    ec->instr_per_second = DEFAULT_INSTR_PER_SECOND;
    ec->frames_per_second = DEFAULT_FRAMES_PER_SECOND;
    ec->timer_decrease_rate = DEFAULT_TIMER_DECREASE_RATE;
    ec->window_width = DEFAULT_WINDOW_WIDTH;
    ec->window_height = DEFAULT_WINDOW_HEIGHT;
    ec->beep_frequency = DEFAULT_BEEP_FREQUENCY;
    ec->rom_path = NULL;
    return CONFIG_INIT_SUCCESS;
}

int emulator_config_initialise_from_args(struct emulator_config *const ec, int argc, char **argv) {
    emulator_config_initialise_default(ec);
    
    if (argc < 2) { 
        return ERR_MISSING_ARGUMENT;
    }
   
    // Code recycled from my CGOL project
    ec->rom_path = argv[1];
    for (int i = 2; i < argc; ++i) { 
        if (!strcmp(argv[i], FLAG_WINDOW_WIDTH)) { 
            errno = 0;
            const char *const w = argv[i+1];
            if (w == NULL) { 
                return ERR_MISSING_ARGUMENT;
            }
            
            long w_long = strtol(w, NULL, 10);
            int is_w_invalid = w_long < MINIMUM_WINDOW_WIDTH || w_long > UINT_MAX;
            if (errno == ERANGE || errno == EINVAL || is_w_invalid) {
                return ERR_INVALID_WINDOW_WIDTH;
            }
            
            ec->window_width = (unsigned)w_long;
            ++i;
        } else if (!strcmp(argv[i], FLAG_WINDOW_HEIGHT)) {
            errno = 0;
            const char *const h = argv[i+1];
            if (h == NULL) { 
                return ERR_MISSING_ARGUMENT;
            }
            
            long h_long = strtol(h, NULL, 10);
            int is_h_invalid = h_long < MINIMUM_WINDOW_HEIGHT || h_long > UINT_MAX;
            if (errno == ERANGE || errno == EINVAL || is_h_invalid) { 
                return ERR_INVALID_WINDOW_HEIGHT;
            }
            
            ec->window_height = (unsigned)h_long;
            ++i;
        } else if (!strcmp(argv[i], FLAG_FPS)) { 
            errno = 0;
            const char *const fps = argv[i+1];
            if (fps == NULL) {
                return ERR_MISSING_ARGUMENT;
            }
            
            char *err;
            double fps_double = strtod(fps, &err);
            int is_fps_invalid = *err != '\0' || fps_double < NON_ZERO_MIN;  
            if (errno == ERANGE || is_fps_invalid) {
               return ERR_INVALID_FPS; 
            }
            
            ec->frames_per_second = fps_double;
            ++i;
        } else if (!strcmp(argv[i], FLAG_IPS)) { 
            errno = 0;
            const char *const ips = argv[i+1];
            if (ips == NULL) {
                return ERR_MISSING_ARGUMENT;
            }
            
            char *err;
            double ips_double = strtod(ips, &err);
            int is_ips_invalid = *err != '\0' || ips_double < NON_ZERO_MIN;
            if (errno == ERANGE || is_ips_invalid) {
                return ERR_INVALID_IPS;
            }
            
            ec->instr_per_second = ips_double;
            ++i;
        } else if (!strcmp(argv[i], FLAG_TIMER_DECREASE_RATE)) {
            errno = 0;
            const char *const tdr = argv[i+1];
            if (tdr == NULL) {
                return ERR_MISSING_ARGUMENT; 
            }
            
            char *err;
            double tdr_double = strtod(tdr, &err);
            int is_tdr_invalid = *err != '\0' || tdr_double < NON_ZERO_MIN;
            if (errno == ERANGE || is_tdr_invalid) {
                return ERR_INVALID_TDR;
            }
            
            ec->timer_decrease_rate = tdr_double;
            ++i;
        } else if (!strcmp(argv[i], FLAG_Q_ARITH_INSTR_OVERFLOW_RESET)) {
            ec->q_arith_instr_overflow_reset = 1;
        } else if (!strcmp(argv[i], FLAG_Q_SHIFT_ONLY_VX)) {
            ec->q_shift_only_vx = 1;
        } else if (!strcmp(argv[i], FLAG_Q_ADD_TO_INDEX_OVERFLOW)) {
            ec->q_add_to_index_overflow = 1;            
        } else if (!strcmp(argv[i], FLAG_Q_STORE_LOAD_INCREMENT_INDEX)) {
            ec->q_store_load_increment_index = 1;
        } else {
            return ERR_UNKNOWN_FLAG; 
        } 
    }
    
    return CONFIG_INIT_SUCCESS;
}