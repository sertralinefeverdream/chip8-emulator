#include "../include/chip8.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Definitions
#define WINDOW_TITLE "Adrian's Chip8 Emulator"
#define WINDOW_WIDTH 1200 
#define WINDOW_HEIGHT 800 
#define SCALE 20
#define ROM_PATH "Breakout.ch8"

// Error codes
#define ERR_SDL_INIT -1
#define ERR_WINDOW_INIT -2
#define ERR_RENDERER_INIT -3

// Config (To be moved into struct later, hardcoded for now)
#define INSTR_PER_SECOND 700.0
#define FRAMES_PER_SECOND 60.0
#define TIMER_DECREASE_RATE 60.0 

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

struct sound_data { 
    double long time;
    double frequency;
    int amplitude;
};

struct sdl_platform { 
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioDeviceID audio_id;
    struct sound_data sd;
};

void beep_samples_generator(void *sd_p, uint8_t *stream, int len) {     
    struct sound_data *sd = (struct sound_data*) sd_p;
    sd->time = 0;
    for (size_t i = 0; i < len; ++i) { 
        stream[i] = sd->amplitude * sin(sd->frequency * 2 * 3.1415 * sd->time);
        //printf("%d\n", stream[i]);
        sd->time += 1.0/44100;
    }
}

int draw_display(const struct sdl_platform *const platform, const struct chip8_machine *const m) {
   SDL_RenderClear(platform->renderer);
   SDL_SetRenderDrawColor(platform->renderer, 255, 255, 255, 255);
   for (size_t x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
       for (size_t y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) { 
           if (m->display[DISPLAY_INDEX(x,y)]) {
               SDL_Rect p;  
               p.x = x * SCALE;
               p.y = y * SCALE;
               p.w = SCALE;
               p.h = SCALE;
               SDL_RenderFillRect(platform->renderer, &p);
           }
       }
   }
   SDL_RenderPresent(platform->renderer);
   SDL_SetRenderDrawColor(platform->renderer, 0, 0, 0, 255);
   return 0;
}

int sdl_platform_init(struct sdl_platform *platform) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) { 
        fprintf(stderr, "Error while initialising SDL.\n");
        return ERR_SDL_INIT;
    }
    
    platform->window = SDL_CreateWindow(
        WINDOW_TITLE, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        WINDOW_WIDTH, 
        WINDOW_HEIGHT, 
        0
    );
    
    if (!platform->window) {
        fprintf(stderr, "Error while creating SDL window.\n");
        return ERR_WINDOW_INIT;
    }
    
    platform->renderer = SDL_CreateRenderer(
        platform->window,
        -1,
        0
    );
    
    if (!platform->renderer) { 
        fprintf(stderr, "Error while creating SDL renderer.\n");
        return ERR_RENDERER_INIT;
    }
    

    platform->sd.frequency = 432;   
    platform->sd.time = 0;
    platform->sd.amplitude = 10;

    SDL_AudioSpec desired_as = {0};
    desired_as.freq = 44100;
    desired_as.format = AUDIO_S8;
    desired_as.channels = 1;   
    desired_as.samples = 1024;    
    desired_as.callback = beep_samples_generator;
    desired_as.userdata = &platform->sd;
    
    SDL_AudioSpec obtained_as;
    platform->audio_id = SDL_OpenAudioDevice(NULL, 0, &desired_as, &obtained_as, 0);
    return 0;
} 

int main(int argc, char **argv) { 
    struct sdl_platform platform;
    if (sdl_platform_init(&platform)) { 
        return EXIT_FAILURE;
    };

    struct chip8_machine *m = chip8_machine_create();
    if (!m) {
        fprintf(stderr, "Error while trying to create chip8_machine");
        return EXIT_FAILURE;
    }

    chip8_machine_load_font(m, fontset);

    if (chip8_machine_load_rom_file(m, ROM_PATH) != ROM_LOAD_SUCCESS) { 
        printf("Error loading ROM");
        return EXIT_FAILURE;
    }
    
    SDL_RenderPresent(platform.renderer);
    uint32_t last_frame_update = 0;
    uint32_t last_timer_update = 0;
    uint32_t last_instr_exec = 0;
    for (;;) {  
        const uint8_t *key_states = SDL_GetKeyboardState(NULL);
        chip8_machine_register_key(m, 0x1, key_states[SDL_SCANCODE_1]);
        chip8_machine_register_key(m, 0x2, key_states[SDL_SCANCODE_2]);
        chip8_machine_register_key(m, 0x3, key_states[SDL_SCANCODE_3]);
        chip8_machine_register_key(m, 0xC, key_states[SDL_SCANCODE_4]);

        chip8_machine_register_key(m, 0x4, key_states[SDL_SCANCODE_Q]);
        chip8_machine_register_key(m, 0x5, key_states[SDL_SCANCODE_W] );
        chip8_machine_register_key(m, 0x6, key_states[SDL_SCANCODE_E]);
        chip8_machine_register_key(m, 0xD, key_states[SDL_SCANCODE_R]);

        chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_A]);
        chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_S]);
        chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_D]);
        chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_F]);

        chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_Z]);
        chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_X]);
        chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_C]);
        chip8_machine_register_key(m, 0x7, key_states[SDL_SCANCODE_V]);
        
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
        if (now - last_instr_exec >= 1000 / INSTR_PER_SECOND) { 
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
        
        if (now - last_timer_update >= 1000 / TIMER_DECREASE_RATE) {
            last_timer_update = now;            
            chip8_machine_decrement_dt(m);
            chip8_machine_decrement_st(m);
        }

        if (now - last_frame_update >= 1000 / FRAMES_PER_SECOND && m->draw_flag) { 
            last_frame_update = now;
            draw_display(&platform, m);
            m->draw_flag = 0;
        }
        
        if (m->sound_timer == 0) {
            SDL_PauseAudioDevice(platform.audio_id, 1);            
        } else {
            SDL_PauseAudioDevice(platform.audio_id, 0);
        }
    }
    
}