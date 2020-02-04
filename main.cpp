#include <stdio.h>
#include "chip8.h"


int main(int argc, char * argv[]) {

    chip8 chip8;

    chip8.load("./rom/Airplane.ch8");

    for (int i = 0; i < 100; i++) {
        chip8.emulate_cycle();
    }

    return 0;
}
