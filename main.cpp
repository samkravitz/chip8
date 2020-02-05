#include <stdio.h>
#include <SDL2/SDL.h>

#include "chip8.h"

bool init();
void close();
void draw_screen(chip8 &);

const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 256;

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Renderer *renderer;
SDL_Surface *gHelloWorld = NULL;

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
    unsigned char *gfx = c.get_gfx();
    c.draw_flag = false;
    SDL_Rect rect;
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 32; x++) {
            SDL_Rect rect;
            rect.x = x * 8;
            rect.y = y * 8;
            rect.w = 8;
            rect.h = 8;


            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            if (gfx[(y*64) + x])
                SDL_RenderFillRect(renderer, &rect);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv) {
    init();

    chip8 chip8;

    chip8.load(argv[1]);

    SDL_Event e;

    // GUI loop
    for (;;) {
        chip8.emulate_cycle();


        if (chip8.draw_flag) draw_screen(chip8);
        while (SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) goto after_loop;

        }
    }

    after_loop:

    close();

    //chip8.load(argv[1]);

    // while (1) {
    //     chip8.emulate_cycle();
    //     if (chip8.draw_flag) chip8.draw();
    // }

    return 0;
}
