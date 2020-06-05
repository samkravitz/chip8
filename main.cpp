#include <stdio.h>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>

#include "chip8.h"

#define NUM_KEYS 16

bool init();
void close();
void draw_screen(chip8 &);

const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 256;

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *gHelloWorld = NULL;

unsigned char screen_buffer[64 * 32];

// This emulator uses this mapping for keys
// Keypad                   Keyboard
// +-+-+-+-+                +-+-+-+-+
// |1|2|3|C|                |1|2|3|4|
// +-+-+-+-+                +-+-+-+-+
// |4|5|6|D|                |Q|W|E|R|
// +-+-+-+-+       =>       +-+-+-+-+
// |7|8|9|E|                |A|S|D|F|
// +-+-+-+-+                +-+-+-+-+
// |A|0|B|F|                |Z|X|C|V|
// +-+-+-+-+                +-+-+-+-+
uint8_t keymap[NUM_KEYS] = {
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_r,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_f,
    SDLK_z,
    SDLK_x,
    SDLK_c,
    SDLK_v,   
};

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

int main(int argc, char **argv) {
    init();
    chip8 chip8;
    chip8.load(argv[1]);
    SDL_Event e;

    // Game loop
    for (;;) {
        chip8.emulate_cycle();
        if (chip8.draw_flag) draw_screen(chip8);
        SDL_PollEvent(&e);

        // check if close button has been clicked
        if (e.type == SDL_QUIT) goto after_loop;

        // check keyboard input
        if (e.type == SDL_KEYDOWN) {
            for (int i = 0; i < NUM_KEYS; ++i) {
                if (e.key.keysym.sym == keymap[i])
                    chip8.key[i] = 1;
            }
            
        }

        // reset keyboard input
        if (e.type == SDL_KEYUP) {
            for (int i = 0; i < NUM_KEYS; ++i) {
                if (e.key.keysym.sym == keymap[i])
                    chip8.key[i] = 0;
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1800)); // slow down emulation speed
    }

    after_loop:
    close();
    return 0;
}
