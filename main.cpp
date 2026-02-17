#include <iostream>
#include <chrono>
#include <thread>

#include "chip8.h"
#include "ioplatform.h"

using namespace std::chrono;


int CLOCK_SPEED = 600; // 600 cycles per second
int FPS = 60; // 60 dispplay frame updates per second => 10 cpu cycles per frame update
int FRAME_TIME = 1000 / FPS;


int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <path-to-rom>" << std::endl;
        return -1;
    }

    // setting up cpu and rom
    Chip8 cpu = Chip8();
    cpu.loadFont();
    std::string fileName = argv[1];
    if (!cpu.loadROM(fileName)) {
        std::cout << "Failed to load ROM" << std::endl;
        return -1;
    }

    // initializing io platform
    IOPlatform io = IOPlatform();

    // initializing running state
    steady_clock::time_point startTime;
    steady_clock::time_point endTime;
    steady_clock::time_point adjustedStartTime;
    steady_clock::time_point adjustedEndTime;
    int running = true;

    while (running) { 
        
        startTime = steady_clock::now();

        // handling input
        int quit = io.getInput(cpu.key);
        if (quit) {
            running = false; 
        }  

        // running clock cycles
        for (int i=0;i<CLOCK_SPEED / FPS;i++) {
            cpu.cycle();
        }


        cpu.updateTimers(); // maintaining timers
    
        io.updateFrame(cpu.display); // update display

        endTime = steady_clock::now();

        // wait to ensure that FRAME_TIME has passed before next iteration
        duration<double> frameDuration = endTime - startTime;
        
        if (frameDuration < milliseconds(FRAME_TIME)){
            std::this_thread::sleep_for(milliseconds(FRAME_TIME) - frameDuration);
        }
        
    }

    return 0;
}