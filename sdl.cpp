#include "sdl.h"

const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 256;

// sdl globals
SDL_Window *gWindow;
SDL_Surface *gScreenSurface;
SDL_Renderer *renderer;
SDL_Surface *gHelloWorld;

unsigned char screen_buffer[64 * 32];

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("could not initialize\n");
        exit(2);
    }

    gWindow = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(gWindow == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    gScreenSurface = SDL_GetWindowSurface(gWindow);
    renderer = SDL_CreateRenderer(gWindow, -1, 0);

    // paint the screen black
    SDL_Rect rect{8, 8, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderPresent(renderer);
    return true;
}

void close() {
    SDL_FreeSurface(gHelloWorld);
    gHelloWorld = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void draw_screen(chip8 &c) {
    c.draw_flag = false;
    SDL_Rect rect;
    rect.w = 8;
    rect.h = 8;
    int index; // current pixel
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            index = y * 64 + x;
            if ((c.gfx[index] | screen_buffer[index]) == 1) { // bit is set
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                rect.x = x * 8;
                rect.y = y * 8;
                SDL_RenderFillRect(renderer, &rect);
            } else if ((c.gfx[index] | screen_buffer[index]) == 0) {
                SDL_SetRenderDrawColor(renderer, 0, 0,0,0);
                rect.x = x * 8;
                rect.y = y * 8;
                SDL_RenderFillRect(renderer, &rect);
            }

            // update screen buffer with current pixel state
            screen_buffer[index] = c.gfx[index];
        }
    }

    SDL_RenderPresent(renderer);
}
