#include "../include/platform.h"
#include "../include/config.h"
#include "../include/chip8.h"

#include <SDL2/SDL.h>
#include <math.h>

void sine_wave_sample(void *config_v, uint8_t *stream, int len) {     
    static unsigned time = 0;
    const struct emulator_config *config = (struct emulator_config*) config_v;
    for (size_t i = 0; i < (size_t)len; ++i) { 
        stream[i] = BEEP_AMPLITUDE * sin(config->beep_frequency * 2 * time * 1/44100.0);
        time++;
    }
}

void play_beep(const struct sdl_platform *const platform) {
    if (platform->audio_id < 2) { 
        return;
    }
    SDL_PauseAudioDevice(platform->audio_id, 0);
}

void stop_beep(const struct sdl_platform *const platform) {
    if (platform->audio_id < 2) {
        return;    
    }
    SDL_PauseAudioDevice(platform->audio_id, 1);
}

int draw_display(const struct sdl_platform *const platform, const struct chip8_machine *const m) {
   SDL_RenderClear(platform->renderer);
   SDL_SetRenderDrawColor(platform->renderer, 255, 255, 255, 255);
   for (size_t x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
       for (size_t y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) { 
           if (m->display[DISPLAY_INDEX(x,y)]) {
               SDL_RenderDrawPoint(platform->renderer, x, y);
           }
       }
   }
   SDL_RenderPresent(platform->renderer);
   SDL_SetRenderDrawColor(platform->renderer, 0, 0, 0, 255);
   return 0;
}

int sdl_platform_init(struct sdl_platform *const platform, const struct emulator_config *const config) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) { 
        fprintf(stderr, "Error while initialising SDL.\n");
        return ERR_SDL_INIT;
    }
    
    platform->window = SDL_CreateWindow(
        WINDOW_TITLE, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        config->window_width, 
        config->window_height, 
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
    
    SDL_RenderSetScale(platform->renderer, config->window_width / (float)CHIP8_DISPLAY_WIDTH, config->window_height / (float)CHIP8_DISPLAY_HEIGHT);

    SDL_AudioSpec desired_as = {0};
    desired_as.freq = 44100;
    desired_as.format = AUDIO_S8;
    desired_as.channels = 1;   
    desired_as.samples = 1024;    
    desired_as.callback = sine_wave_sample;
    desired_as.userdata = config;
    
    SDL_AudioSpec obtained_as;
    platform->audio_id = SDL_OpenAudioDevice(NULL, 0, &desired_as, &obtained_as, 0);
    return 0;
}