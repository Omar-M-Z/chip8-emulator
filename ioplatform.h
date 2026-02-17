#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>
#include <SDL.h>

class IOPlatform {
public:
    IOPlatform();
    void updateFrame(bool frameBuffer[32][64]);
    bool getInput(bool *inputBuffer);

private:
    int DISPLAY_PIXEL_SCALE = 100;
    uint32_t textureSource[32 * 64];

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

};