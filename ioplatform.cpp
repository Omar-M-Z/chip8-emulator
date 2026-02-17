#include "ioplatform.h"
#include <SDL.h>
#include <iostream>


IOPlatform::IOPlatform() {
    const char* title = "Asdf";
    int windowWidth = 64 * 20;
    int windowHeight = 32 * 20;
    window = nullptr;
    renderer = nullptr;

    // setting up window
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(windowWidth, windowHeight, 0, &window, &renderer);

    // initializing texture
    texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        64, 32
    );
    for (int i = 0; i < 64 * 32; i++) {
        textureSource[i] = 0x000000FF; 
    }
    SDL_UpdateTexture(texture, nullptr, textureSource, 64 * sizeof(uint32_t));

    // pushing to display
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);


    SDL_PumpEvents();
}

bool IOPlatform::getInput(bool *inputBuffer) {
    SDL_Event event;
    // iterating through events
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT){
            return true;
        } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP){
            // getting target key
            SDL_Scancode keyCode = event.key.keysym.scancode;
            bool keyState = (event.type == SDL_KEYDOWN);

            // adjusting input buffer
            switch (event.key.keysym.scancode) {
                // row 1
                case SDL_SCANCODE_1: inputBuffer[0x1] = keyState; break;
                case SDL_SCANCODE_2: inputBuffer[0x2] = keyState; break;
                case SDL_SCANCODE_3: inputBuffer[0x3] = keyState; break;
                case SDL_SCANCODE_4: inputBuffer[0xC] = keyState; break;
                // Row 2
                case SDL_SCANCODE_Q: inputBuffer[0x4] = keyState; break;
                case SDL_SCANCODE_W: inputBuffer[0x5] = keyState; break;
                case SDL_SCANCODE_E: inputBuffer[0x6] = keyState; break;
                case SDL_SCANCODE_R: inputBuffer[0xD] = keyState; break;
                // row 3
                case SDL_SCANCODE_A: inputBuffer[0x7] = keyState; break;
                case SDL_SCANCODE_S: inputBuffer[0x8] = keyState; break;
                case SDL_SCANCODE_D: inputBuffer[0x9] = keyState; break;
                case SDL_SCANCODE_F: inputBuffer[0xE] = keyState; break;
                // row 4
                case SDL_SCANCODE_Z: inputBuffer[0xA] = keyState; break;
                case SDL_SCANCODE_X: inputBuffer[0x0] = keyState; break;
                case SDL_SCANCODE_C: inputBuffer[0xB] = keyState; break;
                case SDL_SCANCODE_V: inputBuffer[0xF] = keyState; break;

                default: break;
            }
            return false;
        }
    }
    return false;
}

void IOPlatform::updateFrame(bool frameBuffer[32][64]) {
    // updating texture source
    int textureIndex;
    for (int i=0;i<32;i++){
        for (int j=0;j<64;j++){
            textureIndex = (i * 64) + j;
            if (frameBuffer[i][j]) {
                textureSource[textureIndex] = 0xFFFFFFFF;
            } else {
                textureSource[textureIndex] = 0x000000FF;
            }
        }
    }

    // updating display in window
    SDL_UpdateTexture(texture, nullptr, textureSource, 64 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}