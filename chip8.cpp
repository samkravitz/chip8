#include <stdio.h>
#include <assert.h>

#include "chip8.h"

bool chip8::load(const char *filename) {
    FILE *rom = fopen(filename, "r");
    assert(rom);
    // get size of file
    fseek(rom, 0, SEEK_END);

    long size = ftell(rom);
    rewind(rom);

    // load contents of rom into memory
    for (int i = 0; i < size; ++i) {
        fread(&memory[i + 0x200], sizeof(unsigned char), 1, rom);
    }

    fclose(rom);
    return true;
}

void chip8::emulate_cycle() {
    
}
