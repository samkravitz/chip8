chip8: chip8.cpp chip8.h main.cpp sdl.h sdl.cpp
	g++ -o chip8 chip8.cpp main.cpp sdl.cpp -l SDL2 -lstdc++fs

.PHONY clean:
clean:
	rm -f chip8
