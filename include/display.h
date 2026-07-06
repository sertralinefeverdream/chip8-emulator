#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>

struct display {
    SDL_Window *window;
    SDL_Renderer *renderer;
};

#endif