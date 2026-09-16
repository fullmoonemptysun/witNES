#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <vector>
#include <SDL2/SDL.h>
#include <array>

class Display
{

public:

    Display(){

    }
    void create_display();
    void close_display();
    void refresh_frame(std::vector<std::array<uint8_t, 3>> &); // takes the 256x240 pixel array.
    std::vector<uint8_t> currFrame = std::vector<uint8_t>((61440 * 3), 0);

private:
    SDL_Surface *winSurface = NULL;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
};

#endif
