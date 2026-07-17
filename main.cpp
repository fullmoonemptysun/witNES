#include <iostream>
#include "cpu.h"
#include "Bus.h"
using namespace std;

int main(int argc, char const *argv[])
{

    Bus* cpubus;
    cpubus = new Bus(argv[1]); //creates cpu and cartridge

    // cout << "PC: " << cpubus->witcpu->pc << endl;
    cpubus->witcpu->reset();
    // cout << "BUS,CART,CPU CREATED, STARTING CPU" << endl;
    // cout << toHex(cpubus->witcpu->pc) << endl;
    // while(cpubus->witcpu->pc < 0xc700){

    for(int i = 0; i < 15000; i++){

        // cout << "ENTERED LOOP" <<'\n';
        cpubus->witcpu->clock();
    }

    cout << "DONE!!!!" << endl;

    return 0;   

}
