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
        uint8_t oamdma; //actually belongs to cpu
        uint8_t vreg;
        uint8_t treg;
        uint8_t xreg_ppu;
        uint8_t wreg;

        //2 shift registers
        uint16_t shft_reg_1;
        uint16_t shft_reg_2;

        //Rendering state variables
        int scanline;
        int dot;
        bool bg_render_enable = false;
        bool sp_render_enable = false;
        uint8_t tile_no;
        

        //NMI polling flag
        bool nmi_pending = false;


        //


        void clock();

    
};





#endif
