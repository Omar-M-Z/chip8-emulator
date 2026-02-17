#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>

class Chip8 {
public:
    // io buffers
    bool display[32][64];
    bool key[16];

    // initialziing
    Chip8();
    void loadFont();
    bool loadROM(std::string &fileName);

    // maintenance
    void cycle();
    void updateTimers();

private:

    static const int FONT_OFFSET = 0;
    static const int LOADED_ROM_OFFSET = 0x200;

    // memory
    uint8_t memory[4096];

    // registers
    uint8_t V[16];
    uint16_t I;
    uint16_t PC;

    uint8_t  SP;
    uint16_t stack[16];

    // timers
    uint8_t delayTimer;
    uint8_t soundTimer;

    // running instructions
    uint16_t fetchOpcode();
    void execute(uint16_t opcode);
};