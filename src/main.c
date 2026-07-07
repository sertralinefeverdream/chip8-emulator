#include "../include/chip8.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Definitions
#define WINDOW_TITLE "Adrian's Chip8 Emulator"

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

struct sdl_platform { 
    SDL_Window *window;
    SDL_Renderer *renderer;
};

int sdl_platform_init(struct sdl_platform *platform) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) { 
        fprintf(stderr, "Error while initialising SDL.\n");
        return ERR_SDL_INIT;
    }
    
    platform->window = SDL_CreateWindow(
        WINDOW_TITLE, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        CHIP8_DISPLAY_WIDTH, 
        CHIP8_DISPLAY_HEIGHT,
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
    printf("%d", chip8_machine_load_rom_file(m, "logo.ch8"));

    if (chip8_machine_load_rom_file(m, "logo.ch8") != ROM_LOAD_SUCCESS) { 
        printf("Error loading ROM");
        return EXIT_FAILURE;
    }
    
    // Add logic for loading program here
    
    uint32_t last_frame_update;
    uint32_t last_timer_update;
    uint32_t last_instr_exec;
    for (;;) {  
        printf("Tick");
        uint32_t now = SDL_GetTicks();
        
        // Update display
        if (now - last_frame_update >= 1000 / FRAMES_PER_SECOND && m->draw_flag) { 
            last_frame_update = now;
            m->draw_flag = 0;
        }
        
        if (now - last_instr_exec >= 1000 / INSTR_PER_SECOND) { 
            last_instr_exec = now;
            if (chip8_machine_fetch_and_decode(m)) {
                printf("Fetch and decode failure");
                return EXIT_FAILURE; 
            }
            
            if (chip8_machine_execute(m)) {
               printf("Execute failure");
               return EXIT_FAILURE;
            }
        }
        
        if (now - last_timer_update >= 1000 / TIMER_DECREASE_RATE) {
            last_timer_update = now;            
            chip8_machine_decrement_dt(m);
            chip8_machine_decrement_st(m);
        }
        
        // TBD : Screen Rendering
        // TBD : Keyboard input handling
    }

}