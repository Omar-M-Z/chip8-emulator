all:
	clang++ main.cpp ioplatform.cpp chip8.cpp -o chip8 `sdl2-config --cflags --libs`

clean:
	rm -f chip8