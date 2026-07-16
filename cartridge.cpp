#include "cartridge.h"



using namespace std;

uint8_t Cartridge::cpu_read(uint16_t addr){


    // cout << "CARTRIDGE'S CPU_READ WAS CALLED" << '\n';
    
    switch(mapper){
        case 0x00: //NROM
        // cout << "CPU READING FROM NROM CARTRIDGE" << endl;
            if(header[4] == 2){
                //debug
                // cout << "NROM-256" << '\n';
                data = prg_rom[addr - 0x8000];
            }

            else{
                //debug
                // cout << "NROM-128 - " ;
                if(addr < 0xc000){
                    //debug
                    // cout << "ADDR in limit" << '\n';
                    data = prg_rom[addr - 0x8000];
                }else{
                    //debug
                    // cout << "ADDR in mirror -";
                    data = prg_rom[(addr - 0xc000)];
                    // cout << "DATA: 0x" << toHex(data)  << '\n';
                    
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



