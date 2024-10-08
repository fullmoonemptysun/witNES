#include <cstdint>
#include <array>
#include "Bus.h"

using namespace std;

Bus::Bus(){
    //clear the ram
    for(auto &data : ram){
        data = 0x00;
    }

    cpu.ConnectBus(this);

}

//return type goes before the scope 
void Bus:: write(uint16_t addr, uint8_t data){
    //important range guard
    if(addr >= 0x0000 && addr <= 0xffff){
        ram[addr] = data;
    }
}

uint8_t Bus:: read(uint16_t addr, bool bReadonly){
    //Check if address in the 64kb range
    if(addr >= 0x0000 && addr <= 0xffff){ //important range guard
        return ram[addr];
    }

    else{
        return 0x00;
    }
}





