#include <cstdint>
#include <array>
#include "Bus.h"


using namespace std;

Bus::Bus(const string& filename){
    //clear the ram

    for(auto &data : ram){
        data = 0x00;
    }

    cart = new Cartridge(filename);
    cpu.ConnectBus(this);
    cart->ConnectBus(this);

}

//return type goes before the scope 
void Bus:: write(uint16_t addr, uint8_t data){
    //important range guard
    if(addr >= 0x0000 && addr <= 0xffff){
        //read from the internal ram or its mirrors
        if(addr >= 0x0000 && addr <= 0x07ff){
             ram[addr] = data;
        }

        else if((addr >= 0x0800 && addr <= 0x0fff)) {
            ram[addr - 0x800] = data;
        }

        else if((addr >= 0x1000 && addr <= 0x17ff)) {
            ram[addr - 0x1000] = data;
        }

        else if((addr >= 0x1800 && addr <= 0x1fff)) {
            ram[addr - 0x1800] = data;
        }
    }


}

uint8_t Bus:: read(uint16_t addr, bool bReadonly){
    //Check if address in the 64kb range
    if(addr >= 0x0000 && addr <= 0xffff){ //important range guard

        //read from the internal ram or its mirrors
        if(addr >= 0x0000 && addr <= 0x07ff){
            return ram[addr];
        }

        else if((addr >= 0x0800 && addr <= 0x0fff)) {
            return ram[addr - 0x800];
        }

        else if((addr >= 0x1000 && addr <= 0x17ff)) {
            return ram[addr - 0x1000];
        }

        else if((addr >= 0x1800 && addr <= 0x1fff)) {
            return ram[addr - 0x1800];
        }


        //TODO: read from NES PPU registers region and mirrors

        //TODO: NES APU and I/O register regions
        //TODO: NES APU fun. that is disabled 


        //CARTRIDGE REGIONS
        //0x4020 - 0x5fff (no use ???)
        //TODO: 0x6000 - 0x7fff prg_ram region.

        //prg_rom region
        else if(addr >= 0x8000 && addr <= 0xffff){
            return cart->cpu_read(addr);
        }

        else{
             cout << "ERROR OCCURED. OUT OF ANY ADDRESS REGIONS" << endl;
            return 0;
           
        }


    }

    else{
        cout << "ADDR out of 64kb" << endl;
        return 0x00;
    }
}





