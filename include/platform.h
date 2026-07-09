#ifndef PLATFORM_H
#define PLATFORM_H

#include "../include/config.h"
#include "../include/chip8.h"
#include <SDL2/SDL.h>

// Definitions
#define WINDOW_TITLE "Adrian's Chip8 Emulator"

// Return Codes
#define ERR_SDL_INIT -1
#define ERR_WINDOW_INIT -2
#define ERR_RENDERER_INIT -3

struct sdl_platform { 
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioDeviceID audio_id;
};

void sine_wave_sample(void *config_v, uint8_t *stream, int len);
void play_beep(const struct sdl_platform *const platform);
void stop_beep(const struct sdl_platform *const platform);
int draw_display(const struct sdl_platform *const platform, const struct chip8_machine *const m);
int sdl_platform_init(struct sdl_platform *const platform, const struct emulator_config *const config);

#endif