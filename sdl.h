#ifndef SDL_H
#define SDL_H

#include <SDL2/SDL.h>
#include "chip8.h"

bool init();
void close();
void draw_screen(chip8 &);

#endif // SDL_H
