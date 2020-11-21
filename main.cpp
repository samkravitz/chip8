#include <stdio.h>
#include <chrono>
#include <thread>

#include "chip8.h"
#include "sdl.h"

#define NUM_KEYS 16
//  This emulator uses this mapping for keys
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

int main(int argc, char **argv) {
    init(); // sdl init
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
