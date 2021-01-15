#pragma once

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
  public:
      Beeper();
      ~Beeper();
      void beep(double freq, int duration);
      void generateSamples(Sint16 *stream, int length);
      void wait();

    private:
        double v;
        std::queue<BeepObject> beeps;
};

void audio_callback(void *, Uint8 *, int);
