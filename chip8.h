#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>

class chip8 {

    unsigned char chip8_fontset[80] =
    {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    public:
        chip8() {
            // initialize values
            pc = 0x200; // pc starts at mem address 0x200
            opcode = 0;
            I = 0;
            sp = 0;
            delay_timer = 0;
            sound_timer = 0;
            draw_flag = 0;

            // clear memory
            for (int i = 0; i < 0x1000; ++i) {
                memory[i] = 0;
            }

            // load fontset
            for (int i = 0; i < 80; ++i) {
                memory[i] = chip8_fontset[i];
            }

            // clear stack, registers, key
            for (int i = 0; i < 16; ++i) {
                V[i] = 0;
                stack[i] = 0;
                key[i] = 0;
            }

            // clear graphics
            for (int i = 0; i < 2048; i++) {
                gfx[i] = 0;
            }
        };

        ~chip8() {};

        // signal to update graphics
        bool draw_flag;

        unsigned char key[16]; // key press

        // graphics
        unsigned char gfx[64 * 32]; // 2048 px

        void emulate_cycle();
        bool load(const char *);
        void draw();


    private:
        unsigned char memory[0x1000];
        unsigned char V[16]; // 16 registers
        unsigned short I; // index register
        unsigned short pc; // program counter
        unsigned short opcode;

        unsigned short stack[16];
        unsigned short sp; // stack pointer

        // timer registers
        unsigned char delay_timer;
        unsigned char sound_timer;
};

#endif // CHIP8_H
