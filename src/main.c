#include "../include/chip8.h"
#include "../include/config.h"
#include "../include/platform.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const uint8_t fontset[CHIP8_FONTSET_MAX_SIZE] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void update_keypad(const uint8_t *key_states, struct chip8_machine *const m) {
    chip8_machine_register_key(m, 0x1, key_states[SDL_SCANCODE_1]);
    chip8_machine_register_key(m, 0x2, key_states[SDL_SCANCODE_2]);
    chip8_machine_register_key(m, 0x3, key_states[SDL_SCANCODE_3]);
    chip8_machine_register_key(m, 0xC, key_states[SDL_SCANCODE_4]);

    chip8_machine_register_key(m, 0x4, key_states[SDL_SCANCODE_Q]);
    chip8_machine_register_key(m, 0x5, key_states[SDL_SCANCODE_W] );
    chip8_machine_register_key(m, 0x6, key_states[SDL_SCANCODE_E]);
    chip8_machine_register_key(m, 0xD, key_states[SDL_SCANCODE_R]);

    chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_A]);
    chip8_machine_register_key(m, 0x8, key_states[SDL_SCANCODE_S]);
    chip8_machine_register_key(m, 0x9, key_states[SDL_SCANCODE_D]);
    chip8_machine_register_key(m, 0xE, key_states[SDL_SCANCODE_F]);

    chip8_machine_register_key(m, 0xA, key_states[SDL_SCANCODE_Z]);
    chip8_machine_register_key(m, 0x0, key_states[SDL_SCANCODE_X]);
    chip8_machine_register_key(m, 0xB, key_states[SDL_SCANCODE_C]);
    chip8_machine_register_key(m, 0xF, key_states[SDL_SCANCODE_V]);
}
 
int main(int argc, char **argv) { 
    struct emulator_config config;
    int config_ret = emulator_config_initialise_from_args(&config, argc, argv);
    if (config_ret) {
        char *msg;
        switch (config_ret) {      
            case ERR_MISSING_ARGUMENT:
                msg = "Missing an argument. First argument must be ROM file path.\n";
                break;
            
            case ERR_INVALID_WINDOW_WIDTH:
                msg = "Invalid window width supplied.\n";
                break;
            
            case ERR_INVALID_WINDOW_HEIGHT:
                msg = "Invalid window height supplied.\n";
                break;
            
            case ERR_INVALID_FPS:
                msg = "Invalid fps value supplied.\n";
                break;
                
            case ERR_INVALID_IPS:
                msg = "Invalid instr per second value supplied.\n";
                break;
            
            case ERR_INVALID_TDR: 
                msg = "Invalid timer decrease rate supplied.\n";
                break;
            
            case ERR_UNKNOWN_FLAG:
                msg = "Invalid or unknown flag or arg supplied.\n";
                break;
                
            default:
                msg = "Miscellaneous Error: %d\n";
                break;
        }
        fprintf(stderr, msg, config_ret);
        return EXIT_FAILURE;
    }

    struct sdl_platform platform;
    if (sdl_platform_init(&platform, &config)) { 
        fprintf(stderr, "Error while initialising SDL platform.\n");
        return EXIT_FAILURE;
    };
    
    struct chip8_quirks q = {0};
    q.q_add_to_index_overflow = config.q_add_to_index_overflow; 
    q.q_arith_instr_overflow_reset = config.q_arith_instr_overflow_reset;
    q.q_shift_only_vx = config.q_shift_only_vx;
    q.q_store_load_increment_index = config.q_store_load_increment_index;

    struct chip8_machine *m = chip8_machine_create(q);
    if (!m) {
        fprintf(stderr, "Error while trying to create chip8_machine.\n");
        return EXIT_FAILURE;
    }
    
    chip8_machine_load_font(m, fontset);

    if (chip8_machine_load_rom_file(m, config.rom_path) != ROM_LOAD_SUCCESS) { 
        printf("Error loading ROM. File path may be invalid.\n"); 
        return EXIT_FAILURE;
    }
    
    const uint8_t *key_states = SDL_GetKeyboardState(NULL);
    uint32_t last_frame_update = 0;
    uint32_t last_timer_update = 0;
    uint32_t last_instr_exec = 0;
    SDL_RenderPresent(platform.renderer);

    for (;;) {  
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: 
                    return EXIT_SUCCESS;
                    break;
                
                default:
                    break;
            }
        }
       
        
        uint32_t now = SDL_GetTicks();
        if (now - last_instr_exec >= 1000 / config.instr_per_second) { 
            update_keypad(key_states, m);
            last_instr_exec = now;
            int fetch_return = chip8_machine_fetch_and_decode(m);
            if (fetch_return) {
                printf("Fetch and decode failure: %d", fetch_return);
                return EXIT_FAILURE; 
            }
            
            int decode_return = chip8_machine_execute(m);
            if (decode_return) {
               printf("Execute failure: %d", decode_return);
               return EXIT_FAILURE;
            }
        }
        
        if (now - last_timer_update >= 1000 / config.timer_decrease_rate) {
            last_timer_update = now;            
            chip8_machine_decrement_dt(m);
            chip8_machine_decrement_st(m);
        }

        if (now - last_frame_update >= 1000 / config.frames_per_second && m->draw_flag) { 
            last_frame_update = now;
            draw_display(&platform, m);
            m->draw_flag = 0;
        }
        
        if (m->sound_timer) {
            play_beep(&platform);
        } else {
            stop_beep(&platform);
        }
    }
    
}