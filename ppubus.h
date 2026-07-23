#ifndef PPUB_H
#define PPUB_H

#include <vector>
#include <cstdint>
#include "ppu.h"



class PPUBus{

    public:
        PPUBus(){
            witppu = new ppu();
        }
        

        uint8_t read_register(uint16_t addr);
        void write_register(uint16_t addr, uint8_t data, uint16_t totalCycles);

        ppu* witppu;

        uint8_t vram[10000]; //vram 10 kilobytes
        uint8_t latch = 0x00; //latch value of this bus (last read/write)


};




#endif
