#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include "ppubus.h"
#include <string>
#include <array>

class ppu
{
public:
    ppu()
    {
    }

    // registers
    uint8_t ppuctrl;
    uint8_t ppumask;
    uint8_t ppustatus;
    uint8_t oamaddr;
    uint8_t oamdata;
    uint8_t ppuscroll;
    uint8_t ppuaddr;
    uint8_t ppudata;
    uint8_t oamdma; // actually belongs to cpu
    uint16_t vreg;
    uint16_t treg;
    uint16_t xreg_ppu;
    uint8_t wreg;

    // 3 shift registers
    uint16_t shft_reg_hi;
    uint16_t shft_reg_lo;
    uint16_t attr_reg_hi;
    uint16_t attr_reg_lo;

    // Rendering state variables
    int scanline;
    int dot;
    bool bg_render_enable = false;
    bool sp_render_enable = false;
    uint8_t tile_no;

    // NMI polling flag
    bool nmi_pending = false;

    //

    void clock();

    void ConnectBus(PPUBus *b)
    {
        this->bus = b;
    }

private:
    PPUBus *bus;

    uint8_t read(uint16_t addr)
    {
        return bus->read_mem(addr);
    }

    void write(uint16_t addr, uint8_t data)
    {
        bus->write_mem(addr, data);
    }

    uint8_t read_nt(int);
    uint8_t read_at(int);
    uint16_t read_pt(int);

    

    //0 for sprite, 1 for bg, 2 for EXT
    uint8_t pmux[2][2][2] = {
        {{2, 2}, {0, 0}},
        {{1, 1}, {0, 1}}
    };


    
};

#endif
