#ifndef SDL_H
#define SDL_H

#include <SDL2/SDL.h>
#include <queue>
#include "chip8.h"

// video functions
bool init();
void close();
void draw_screen(chip8 &);

struct BeepObject {
    double freq;
    int samplesLeft;
};

class Beeper {
    private:
        double v;
        std::queue<BeepObject> beeps;
    public:
        Beeper();
        ~Beeper();
        void beep(double freq, int duration);
        void generateSamples(Sint16 *stream, int length);
        void wait();
};

void audio_callback(void *, Uint8 *, int);

#endif // SDL_H
