#include "sdl.h"

#include <cstdint>

// video constants
constexpr int SCREEN_WIDTH = 512;
constexpr int SCREEN_HEIGHT = 256;

// sound constants
constexpr int AMPLITUDE = 28000;
constexpr int FREQUENCY = 44100;

// sdl globals
SDL_Window *gWindow;
SDL_Surface *gScreenSurface;
SDL_Renderer *renderer;
SDL_Surface *gHelloWorld;

uint8_t screen_buffer[64 * 32];

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

// Beeper constructor
Beeper::Beeper() {
    SDL_AudioSpec desiredSpec;

    desiredSpec.freq = FREQUENCY;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;

    // you might want to look for errors here
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);

    // start play audio
    SDL_PauseAudio(0);
}

Beeper::~Beeper() {
    SDL_CloseAudio();
}

void audio_callback(void *_beeper, Uint8 *_stream, int length) {
    Sint16 *stream = (Sint16*) _stream;
    length = length / 2;
    Beeper *beeper = (Beeper *) _beeper;

    beeper->generateSamples(stream, length);
}

void Beeper::generateSamples(Sint16 *stream, int length) {
    int i = 0;
    while (i < length) {

        if (beeps.empty()) {
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }
        BeepObject& bo = beeps.front();

        int samplesToDo = std::min(i + bo.samplesLeft, length);
        bo.samplesLeft -= samplesToDo - i;

        while (i < samplesToDo) {
            stream[i] = AMPLITUDE * std::sin(v * 2 * M_PI / FREQUENCY);
            i++;
            v += bo.freq;
        }

        if (bo.samplesLeft == 0) {
            beeps.pop();
        }
    }
}

void Beeper::beep(double freq, int duration) {
    BeepObject bo;
    bo.freq = freq;
    bo.samplesLeft = duration * FREQUENCY / 1000;

    SDL_LockAudio();
    beeps.push(bo);
    SDL_UnlockAudio();
}

void Beeper::wait() {
    int size;
    do {
        SDL_Delay(20);
        SDL_LockAudio();
        size = beeps.size();
        SDL_UnlockAudio();
    } while (size > 0);

}
