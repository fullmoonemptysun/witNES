#include <iostream>
#include "cpu.h"
#include "Bus.h"
using namespace std;

int main(int argc, char const *argv[])
{


    Bus bus(argv[1]); //creates cpu and cartridge
    cout << bus.witcpu.pc << endl;
    bus.witcpu.reset();
    cout << "BUS CREATED, STARTING CPU" << endl;
    cout << bus.witcpu.pc << endl;
    // while(bus.witcpu.pc < 0xc6b5){

    //     cout << "ENTERED LOOP";
    //     bus.witcpu.clock();
    // }


    return 0;
    
    
        
        

}
