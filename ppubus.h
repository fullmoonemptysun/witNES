#ifndef PPUB_H
#define PPUB_H

#include <vector>
#include <cstdint>
#include <array>
#include "ppu.h"



class PPUBus{

    public:
        PPUBus(){
            witppu = new ppu();
        }
        

        uint8_t read_register(uint16_t addr);
        void write_register(uint16_t addr, uint8_t data, uint16_t totalCycles);
        uint8_t read_mem(uint16_t);
        

        ppu* witppu;

        array<uint8_t, 2 * 1024> vram; //vram 2 kilobytes
        array<uint8_t, 64> oam;
        
    
        uint8_t latch = 0x00; //latch value of the bus (last read/write)


};




#endif
