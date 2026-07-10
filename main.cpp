#include <iostream>
#include "cpu.h"
#include "Bus.h"
using namespace std;

int main(int argc, char const *argv[])
{


    Bus bus(argv[1]); //creates cpu and cartridge



    while(bus.witcpu.pc < 0xc6b5){
        bus.witcpu.clock();
    }
    
    
        
        

}
