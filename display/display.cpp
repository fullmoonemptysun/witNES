#include <iostream>
#include <SDL2/SDL.h>
#include "display.h"
#include <fstream>


#include <cstdint>

using namespace std;

void Display::create_display()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "ERROR: COULD NOT INITIALIZE SDL" << SDL_GetError() << endl;

        system("sleep 2");
        exit(-1);
    }

    window = SDL_CreateWindow("WITNES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);

    if (!window)
    {
        cout << "ERROR: Creating window" << SDL_GetError() << endl;
        system("sleep 2");
        exit(-1);
    }

   

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        cout << "ERROR: CReating renderer" << SDL_GetError() << endl;
        system("sleep 2");
        exit(-1);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    if (!texture)
    {
        cout << "ERROR: CReating texture" << SDL_GetError() << endl;
        system("sleep 2");
        exit(-1);
    }
}

void Display::refresh_frame(vector<array<uint8_t, 3>> &framebuf)
{ // flatten the 2d array into a contiguous block of bytes
     for (int i = 0; i < framebuf.size(); i++)
     {
         for (int j = 0; j < 3; j++)
         {
             currFrame.push_back(framebuf[i][j]);

            
     }
         }

    //put the bitmap to the texture;
    SDL_UpdateTexture(texture, NULL, currFrame.data(), 256 * 3);


    //present 
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void Display::close_display()
{
    SDL_DestroyWindow(window);
    window = NULL;
    winSurface = NULL;
    SDL_Quit();
}




//for testing the display
// int main(int argc, char**argv){

//     Display* screen = new Display();
//     ifstream file("pixels.rgb", ios::binary);
//     if(!file.is_open()){
//         std::cerr << "COULD NOT READ THE FILE" << endl;
//         exit(-1);
//     }
//     file.read((char*)screen->currFrame.data(), (61440*3));
    
    
  
//     screen->create_display();
//     // screen->refresh_frame();
//     system("sleep 5");
//     screen->close_display();
//     return 0;
// }
