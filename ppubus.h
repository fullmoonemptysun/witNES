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
        void write_register(uint16_t addr, uint8_t data);

        ppu* witppu;

        uint8_t vram[10000]; //vram 10 kilobytes


};




#endif
