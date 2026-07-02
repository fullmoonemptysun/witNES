#include "cartridge.h"



using namespace std;

uint8_t Cartridge::cpu_read(uint16_t addr){
    
    switch(mapper){
        case 0x00: //NROM
            if(header[4] == 2){
                data = prg_rom[addr - 0x8000];
            }

            else{
                if(addr < 0xc000){
                    data = prg_rom[addr - 0x8000];
                }else{
                    data = prg_rom[(addr - 0xc000)];
                }
            }

            break;
        

            
    }

    return data;
}   


uint8_t Cartridge::cpu_write(uint16_t addr, uint8_t value){
    cout << "BAD: TRYING TO WRITE TO PRG_ROM" <<'\n';
    return 0;
}



