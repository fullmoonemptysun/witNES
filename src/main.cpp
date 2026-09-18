#include <iostream>
#include "cpu.h"
#include "Bus.h"
#include "SDL2/SDL.h"

using namespace std;

int main(int argc, char const *argv[])
{

    Bus *cpubus;

    cpubus = new Bus(argv[1]); // creates cpu and cartridge

    // cout << "PC: " << cpubus->witcpu->pc << endl;
    cpubus->witcpu->reset();
    // cout << "BUS,CART,CPU CREATED, STARTING CPU" << endl;
    // cout << toHex(cpubus->witcpu->pc) << endl;
    // while(cpubus->witcpu->pc < 0xc700){
    int count = 0;

    while (true)
    {

        // cout << "ENTERED LOOP" <<'\n';
        cpubus->ppub->witppu->clock();
        cpubus->ppub->witppu->clock();
        cpubus->ppub->witppu->clock();
        cpubus->witcpu->clock();
        count += 1;

        if (count == 20500)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    // handle quit

                    exit(-2);
                }
            }

            count = 0;
        }
    }

    cout << "TOTAL CYCLES: " << cpubus->witcpu->cputicks << '\n';
    cout << "DONE!!!!" << endl;

    return 0;
}
