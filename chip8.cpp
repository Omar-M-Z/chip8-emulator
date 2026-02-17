#include "chip8.h"
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>


Chip8::Chip8(){
    // initialzing memory and state
    std::memset(memory, 0, sizeof(memory));
    std::memset(V, 0, sizeof(V));
    std::memset(stack, 0, sizeof(stack));
    std::memset(display, 0, sizeof(display));
    std::memset(key, 0, sizeof(key));
    I  = 0;
    PC = LOADED_ROM_OFFSET;
    SP = 0;
    delayTimer = 0;
    soundTimer = 0;
}


void Chip8::loadFont()
{
    uint8_t fontset[80] = {
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

    for (int i = 0; i < 80; ++i) {
        memory[FONT_OFFSET + i] = fontset[i];
    }
}

bool Chip8::loadROM(std::string &fileName) {
    std::ifstream file = std::ifstream(fileName, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos size = file.tellg();

        if (size > 4096 - LOADED_ROM_OFFSET ) {
            std::cout << "Error: ROM file is too large\n" << std::endl;  
            return false;
        }

        char* buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        

        for (int i = 0; i < size; ++i) {
            memory[LOADED_ROM_OFFSET + i] = static_cast<uint8_t>(buffer[i]);
        }

        delete[] buffer;


        return true;

    }
    return false;
}

void Chip8::updateTimers() {
    if (delayTimer > 0) {
        delayTimer--;
    }
    if (soundTimer > 0) {
        soundTimer--;
        // TODO: trigger a beep sound here if the timer is > 0
    }
}

uint16_t Chip8::fetchOpcode() {
    // getting 2 bytes of data starting from the program counter
    return (memory[PC] << 8) | memory[PC + 1];
}

void Chip8::execute(uint16_t opcode) {
    // getting variables
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t n = opcode & 0x000F;
    uint8_t x = (opcode >> 8) & 0x0F;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;

    PC += 2; // incrementing pc to next instruction

    // 00E0 , clearing screen
    if (opcode == 0x00E0) {
        std::memset(display, 0, sizeof(display));
        return;
    }
    // 00EE, returning from subroutine
    if (opcode == 0x00EE) {
        SP--;
        PC = stack[SP];
        return;
    }
    // 1NNN, jumping to address nnn
    if ((opcode & 0xF000) == 0x1000) {
        PC = nnn;
        return;
    }
    // 2NNN, calling subroutine at nnn
    if ((opcode & 0xF000) == 0x2000) {
        stack[SP] = PC;
        SP++;
        PC = nnn;
        return;
    }
    // 3XKK,        skipping next instruction if register V[x] == kk
    if ((opcode & 0xF000) == 0x3000){
        if (V[x] == kk){
            PC += 2;
        }
    } 
    // 4xkk, skip next instruction if Vx != kk
    if ((opcode & 0xF000) == 0x4000){
        if (V[x] != kk){
            PC += 2;
        }
    }
    // 5xy0, skip next instruction if Vx = Vy
    if ((opcode & 0xF00F) == 0x5000){
        if (V[x] == V[y]){
            PC += 2;
        }
    }
    // 6xkk, load value kk into reg Vx
    if ((opcode & 0xF000) == 0x6000){
        V[x] = kk;
    }
    // 7xkk, adding kk to Vx
    if ((opcode & 0xF000) == 0x7000){
        V[x] += kk;
    }
    // 8xy0, store value of Vy in Vx
    if ((opcode & 0xF00F) == 0x8000){
        V[x] = V[y];
    }
    // 8xy1, set Vx = Vx OR Vy.
    if ((opcode & 0xF00F) == 0x8001){
        V[x] = V[x] | V[y];
    }
    // 8xy2, set Vx = Vx AND Vy.
    if ((opcode & 0xF00F) == 0x8002){
        V[x] = V[x] & V[y];
    }

    // 8xy3, set Vx = Vx XOR Vy
    if ((opcode & 0xF00F) == 0x8003){
        V[x] = V[x] ^ V[y];
    }


    // 8xy4, add Vy to Vx, set VF = carry
    if ((opcode & 0xF00F) == 0x8004){
        uint16_t sum = V[x] + V[y];
        V[0xF] = (sum > 255);
        V[x] = sum & 0xFF;
    }

    // 8xy5, subtract Vy from Vx, set VF = NOT borrow
    if ((opcode & 0xF00F) == 0x8005){
        V[0xF] = (V[x] > V[y]);
        V[x] = V[x] - V[y];
    }

    // 8xy6, shift Vx right by 1
    if ((opcode & 0xF00F) == 0x8006){
        V[0xF] = V[x] & 0x1;
        V[x] >>= 1;
    }

    // 8xy7, set Vx = Vy - Vx
    if ((opcode & 0xF00F) == 0x8007){
        V[0xF] = (V[y] > V[x]);
        V[x] = V[y] - V[x];
    }

    // 8xyE, shift Vx left by 1
    if ((opcode & 0xF00F) == 0x800E){
        V[0xF] = (V[x] & 0x80) >> 7;
        V[x] <<= 1;
    }

    // 9xy0, skip next instruction if Vx != Vy
    if ((opcode & 0xF00F) == 0x9000){
        if (V[x] != V[y]){
            PC += 2;
        }
    }

    // Annn, set I = nnn
    if ((opcode & 0xF000) == 0xA000){
        I = nnn;
    }

    // Bnnn, jump to location nnn + V0
    if ((opcode & 0xF000) == 0xB000){
        PC = nnn + V[0];
    }

    // Cxkk, set Vx = random byte AND kk
    if ((opcode & 0xF000) == 0xC000){
        V[x] = (rand() % 256) & kk;
    }
    
    // Dxyn, draw sprite that is n bytes starting at I, and at display position x, y
    if ((opcode & 0xF000) == 0xD000){
        V[0xF] = 0;
        uint8_t startX = V[x];
        uint8_t startY = V[y];
        for (uint8_t row = 0; row < n; row++){
            uint8_t spriteByte = memory[I + row];
            for (int col=0;col<8;col++){

                uint8_t screenX = (startX + col) % 64;
                uint8_t screenY = (startY + row) % 32;
                // writing to the screen
                if (((spriteByte >> (7-col)) & 0x1) != 0){
                    // setting V[0xf] if having to erase previous
                    if (display[screenY][screenX] == 1){
                        V[0xF] = 1;
                    }
                    // writing to screen using XOR
                    display[screenY][screenX] ^= 1;
                }
            }
        }
    }

    // Ex9E, skip if key Vx is pressed
    if ((opcode & 0xF0FF) == 0xE09E){
        if (key[V[x]]){
            PC += 2;
        }
    }

    // ExA1, skip if key Vx is NOT pressed
    if ((opcode & 0xF0FF) == 0xE0A1){
        if (!key[V[x]]){
            PC += 2;
        } else {
            std::cout << "key " << (int)V[x] << " being pressed\n"; 
        }
    }

    // Fx07, set Vx = delay timer
    if ((opcode & 0xF0FF) == 0xF007){
        V[x] = delayTimer;
    }

    // Fx0A, wait for key press and release, store value in Vx

   if ((opcode & 0xF0FF) == 0xF00A) {
        static int keyAwaitingRelease = -1;
        // waiting for a key to be pressed down
        if (keyAwaitingRelease == -1) {
            for (int i = 0; i < 16; i++) {
                if (key[i]) {
                    keyAwaitingRelease = i;
                    break;
                }
            }
            PC -= 2;
        }
        // waiting for the previously pressed key to be released
        else {
            if (!key[keyAwaitingRelease]) {
                V[x] = keyAwaitingRelease;
                keyAwaitingRelease = -1;
            } else {
                PC -= 2;
            }
        }
    }


    // Fx15, set delay timer = Vx
    if ((opcode & 0xF0FF) == 0xF015){
        delayTimer = V[x];
    }

    // Fx18, set sound timer = Vx
    if ((opcode & 0xF0FF) == 0xF018){
        soundTimer = V[x];
    }

    // Fx1E, add Vx to I
    if ((opcode & 0xF0FF) == 0xF01E){
        I += V[x];
    }


    // Fx29, set I to sprite location for digit Vx
    if ((opcode & 0xF0FF) == 0xF029){
        I = FONT_OFFSET + (V[x] * 5);
    }

    // Fx33, store BCD of Vx
    if ((opcode & 0xF0FF) == 0xF033){
        memory[I] = V[x] / 100;
        memory[I + 1] = (V[x] % 100) / 10;
        memory[I + 2] = V[x] % 10;
    }


    // Fx55, store V0 through Vx in memory
    if ((opcode & 0xF0FF) == 0xF055){
        for (int i = 0; i <= x; i++){
            memory[I + i] = V[i];
        }
    }

    // Fx65, load V0 through Vx from memory
    if ((opcode & 0xF0FF) == 0xF065){
        for (int i = 0; i <= x; i++){
            V[i] = memory[I + i];
        }
    }

}


void Chip8::cycle() {
    uint16_t opcode = fetchOpcode();
    execute(opcode);
}