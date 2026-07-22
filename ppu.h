#ifndef PPU_H
#define PPU_H

#include <cstdint>

#include <string>

class ppu {
    public:
        ppu(){

        }
            

   


        //registers
        uint8_t ppuctrl;
        uint8_t ppumask;
        uint8_t ppustatus;
        uint8_t oamaddr;
        uint8_t oamdata;
        uint8_t ppuscroll;
        uint8_t ppuaddr;
        uint8_t ppudata;
        uint8_t oamdma; //?

    
};





#endif
