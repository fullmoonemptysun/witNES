#include <iostream>
#include <SDL2/SDL.h>
#include "display.h"


using namespace std;


void Display::create_display(){
    SDL_Surface* winSurface = NULL;
    SDL_Window* window = NULL;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        cout << "ERROR: COULD NOT INITIALIZE SDL" << SDL_GetError() << endl;\

        system("sleep 2");
        exit(-1);
    }


    window = SDL_CreateWindow("WITNES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);

    if(!window){
        cout << "ERROR: Creating window" << SDL_GetError() << endl;
        system("sleep 2");
        exit(-1);
    }

    winSurface = SDL_GetWindowSurface(window);

    if(!winSurface){
        cout << "ERROR: Getting Surface" << SDL_GetError() << endl;

        system("sleep 2");
        exit(-1);
    }

}


void Display::refresh_frame(vector<vector<uint8_t>>& framebuf){

}



