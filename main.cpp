#include <stdio.h>
#include "chip8.h"


int main(int argc, char * argv[]) {

    chip8 chip8;

    chip8.load("./rom/Airplane.ch8");
    return 0;
}
