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

    // 0 for sprite, 1 for bg, 2 for EXT
    uint8_t pmux[2][2][2] = {
        {{2, 2}, {0, 0}},
        {{1, 1}, {0, 1}}};

    // frame bitmap
    array<array<uint8_t, 3>, 61440> frame;

    //master palette
    array<array<uint8_t, 3> , 64> mpallette = {{
        {66, 71, 74},{0, 10, 114},{6, 0, 133},{42, 0, 120},{68, 0, 78},{79, 0, 17},{72, 0, 0},{48, 11, 0},

        {14, 32, 0},{0, 47, 0},{0, 53, 0},{0, 46, 4},{0, 30, 66},{0, 30, 66},{0, 30, 66},{0, 30, 66},

        {75, 81, 84},{0, 52, 174},{14, 0, 186},{82, 3, 194},{118, 0, 144},{136, 0, 70},{130, 19, 0},{102, 46, 0},

        {60, 73, 0},{15, 94, 0},{0, 103, 0},{0, 97, 28},{0, 78, 109},{0, 0, 0},{0, 0, 0},{0, 0, 0},

        {24, 26, 27},{0, 75, 160},{0, 5, 131},{70, 0, 145},{122, 0, 153},{149, 7, 102},{142, 14, 7},{176, 109, 13},

        {139, 135, 0},{96, 157, 0},{59, 168, 14},{38, 166, 80},{38, 151, 157},{49, 53, 55},{0, 0, 0},{0, 0, 0},

        {24, 26, 27},{44, 47, 49},{39, 42, 44},{40, 43, 45},{68, 0, 89},{86, 4, 67},{83, 3, 9},{97, 55, 11},

        {78, 73, 20},{67, 81, 17},{60, 99, 18},{19, 88, 49},{25, 87, 83},{67, 72, 75},{0, 0, 0},{0, 0, 0}

    }};
};

#endif
