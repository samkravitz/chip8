//#include <fstream>
#include <iostream>
#include <experimental/filesystem>
#include <string.h>

#include "chip8.h"
#include "sdl.h"

namespace fs = std::experimental::filesystem;

#define INSTRUCTION_LENGTH 2

// audio
Beeper beeper;
const double A = 440.0; // beeping frequency
const int DURATION = 140;

chip8::chip8() {
  // initialize values
  pc = 0x200; // pc starts at mem address 0x200
  opcode = 0;
  I = 0;
  sp = 0;
  delay_timer = 0;
  sound_timer = 0;
  draw_flag = 0;

  // clear memory
  for (int i = 0; i < 0x1000; ++i)
      memory[i] = 0;

  // load fontset
  for (int i = 0; i < 80; ++i)
      memory[i] = chip8_fontset[i];

  // clear graphics
  for (int i = 0; i < 2048; i++)
      gfx[i] = 0;

  // clear stack, registers, keys
  for (int i = 0; i < 16; ++i) {
      V[i] = 0;
      stack[i] = 0;
      key[i] = 0;
  }

}

bool chip8::load(const std::string &filename) {
    std::ifstream rom(filename, std::ios::in | std::ios::binary);

    if (!rom || !rom.good())
    {
        std::cerr << "Error loading ROM: " << filename << std::endl;
        exit(0);
    }

    size_t rom_size = fs::file_size(filename);

    rom.read((char *) &memory[0x200], rom_size);
    rom.close();

    return true;
}

void chip8::emulate_cycle() {
    // fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // decode opcode
    switch (opcode & 0xF000) {
        // TODO: case 0s
        case 0x0000:
            switch (opcode & 0x000F) {
                // 00E0 - clears the screen
                case 0x0000:
                    for (int i = 0; i < 2048; i++) {
                        gfx[i] = 0;
                    }
                    draw_flag = true;
                    pc += 2;
                    break;

                // 00EE - returns from subroutine
                case 0x000E:
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;

                default:
                    printf ("Unknown opcode 0: 0x%X\n", opcode);
                    exit(1);
                    break;
            }
            break;

        // 1NNN - jumps to address NNN
        case 0x1000:
            pc = opcode & 0x0FFF;
            break;

        // 2NNN - calls subroutine at NNN
        case 0x2000:
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;

        // 3XNN - skips next instruction if VX = NN
        case 0x3000:
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        // 4XNN - skips next instruction if VX != NN
        case 0x4000:
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        // 5XY0 - skips next instruction if VX = VY
        case 0x5000:
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        // 6XNN - sets VX to NN
        case 0x6000:
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;

        // 7XNN - adds NN to VX (VX = VX + NN)
        case 0x7000:
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;

        // 8XY_ - arithmetic
        case 0x8000:
            switch (opcode & 0x000F) {
                // 8XY0 - sets VX to the value of VY
                case 0x0000:
                    V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY1 - sets VX to VX ~or~ VY
                case 0x0001:
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY2 - sets VX to VX ~and~ VY
                case 0x0002:
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY3 - sets VX to VX ~xor~ VY
                case 0x0003:
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY4 - adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
                case 0x0004:
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
                        V[0xF] = 1; // VY > 255 - VX, carry
                    } else {
                        V[0xF] = 0;
                    }
                    pc += 2;
                    break;

                // 8XY5 - subtracts VY from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                case 0x0005:
                    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
                        V[0xF] = 0; // VY > VX, (ie, VX - VY < 0), so theres a borrow
                    } else {
                        V[0xF] = 1; // no borrow
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY6 - stores the least significant bit of VX in VF and then shifts VX to the right by 1
                case 0x0006:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;

                // 8XY7 - sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                case 0x0007:
                    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
                        V[0xF] = 0; // VX > VY, (ie, VY - VX < 0), so theres a borrow
                    } else {
                        V[0xF] = 1; // no borrow
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8]; // VY - VX
                    pc += 2;
                    break;

                // 8XYE - stores the most significant bit of VX in VF and then shifts VX to the left by 1
                case 0x000E:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7; // most significant bit
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;

                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
                    exit(1);
                    break;
            }
            break;

        // 9XY0 - skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
        case 0x9000:
            pc += V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4] ? 4 : 2;
            break;

        // ANNN - sets I to the address NNN
        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        // BNNN  - jumps to the address NNN plus V0
        case 0xB000:
            pc += (opcode & 0x0FFF) + V[0x0];
            break;

        // CXNN - sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
        case 0xC000:
        {
            int r = rand() % 256;
            V[(opcode & 0x0F00) >> 8] = r & (opcode & 0x00FF);
            pc += 2;
            break;
        }

        // DXYN - update graphics
        case 0xD000:
        {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (gfx[(x + xline + ((y + yline) * 64))] == 1) V[0xF] = 1;
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            draw_flag = true;
            pc += 2;
            break;
        }

        // EXY_ key opcodes
        case 0xE000:
            switch (opcode & 0x000F) {
                // EX9E - skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
                case 0x000E:
                    if (key[V[(opcode & 0x0F00) >> 8]]) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;

                // EXA1 - skips the next instruction if the key stored in VX isn't pressed
                case 0x0001:
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;

                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
                    exit(1);
                    break;
            }
            break;

        // FXY_ miscellaneous
        case 0xF000:
            switch (opcode & 0x00FF) {
                // FX07 - sets VX to the value of the delay timer
                case 0x0007:
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;

                // FX0A - a key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
                case 0x000A:
                {
                    bool key_pressed = 0;
                    for (int i = 0; i < 16; ++i) {
                        if (key[i]) {
                            V[(opcode & 0x0F00) >> 8] = i;
                            key_pressed = true;
                        }
                    }

                    if (!key_pressed) return;

                    pc += 2;
                }
                break;

                // FX15 - sets the delay timer to VX
                case 0x0015:
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                // FX18 - sets the sound timer to VX
                case 0x0018:
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                // FX1E - Adds VX to I VF is set to 1 when there is a range overflow, and 0 when there isnt
                case 0x001E:
                    if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                // FX29 - Sets I to the location of the sprite for the character in VX
                case 0x0029:
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;

                // FX33 - take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
                case 0x0033:
        					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
        					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
        					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
        					pc += 2;
        				    break;

                // FX55 - stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
                case 0x0055:
                    for (int i = 0; i < 16; ++i) {
                        memory[I + i] = V[i];
                    }
                    pc += 2;
                    break;

                // FX65 - fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
                case 0x0065:
                    for (int i = 0; i < 16; ++i) {
                         V[i] = memory[I + i];
                    }
                    pc += 2;
                    break;

                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
                    exit(1);
                    break;


            }
            break;

        default:
            printf ("Unknown opcode: 0x%X\n", opcode);
            exit(1);
            break;
    }

    // update timers
    if(delay_timer > 0) --delay_timer;

    if(sound_timer > 0) {
        if(sound_timer == 1) {
            beeper.beep(A, DURATION);
            beeper.wait();
        }
    --sound_timer;
    }
}
