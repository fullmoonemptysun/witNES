#include <iostream>
#include "cpu.h"
#include "Bus.h"
using namespace std;

int main(int argc, char const *argv[])
{


    Bus* cpubus;

    cpubus = new Bus(argv[1]); //creates cpu and cartridge
    cout << cpubus->witcpu->pc << endl;
    cpubus->witcpu->reset();
    cout << "BUS CREATED, STARTING CPU" << endl;
    cout << (int)(cpubus->witcpu->pc) << endl;
    while(cpubus->witcpu->pc < 0xc6b5){

        cout << "ENTERED LOOP";
        cpubus->witcpu->clock();
    }


    return 0;
    
    
        
        

}
