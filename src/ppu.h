#ifndef PPU_H
#define PPU_H

#define CAST_15(x) ((x) & 0x7FFF)
#define CAST_14(x) ((x) & 0x3fff)
#define CAST_5(x) ((x) & 0x1F)
#define CAST_6(x) ((x) & 0x3F)
#define RENDERING_ENABLE (sp_render_enable || bg_render_enable)

#include <cstdint>

#include <string>
#include <vector>
#include <array>
#include "display.h"

using namespace std;


class PPUBus;
class ppu
{
public:
    ppu(){

        screen = new Display();
        screen->create_display();
        
        
    };
    ~ppu(){
        delete screen;
    };


    void reset();

    // registers
    uint8_t ppuctrl = 0;
    uint8_t ppumask = 0;
    uint8_t ppustatus = 0;
    uint8_t oamaddr = 0;
    uint8_t oamdata = 0;
    uint8_t ppuscroll = 0;
    uint8_t ppuaddr = 0;
    uint8_t ppudata = 0;
    uint8_t pdatabuf = 0; //buffer for ppudata (1 step slow writes)
    uint8_t oamdma = 0; // actually belongs to cpu
    uint16_t vreg = 0;
    uint16_t treg = 0;
    uint8_t xreg_ppu = 0;
    uint8_t wreg = 0;


    // 3 shift registers
    uint16_t shft_reg_hi = 0;
    uint16_t shft_reg_lo = 0;
    uint16_t attr_reg_hi = 0;
    uint16_t attr_reg_lo = 0;

    // Rendering state variables
    int scanline = 0;
    int dot = 0;
    bool bg_render_enable = false;
    bool sp_render_enable = false;
    uint8_t tile_no; //???

    // NMI polling flag
    bool nmi_pending = false;

    // TODO: grayscale mode. Not needed for NROM games, maybe implemented in the future
    bool grayscale = false;

    void clock();
    PPUBus *bus;

    void ConnectBus(PPUBus *b)
    {
        this->bus = b;
    }

    Display* screen;

private:
    

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

    uint8_t read_nt(int);
    uint8_t read_at(int);
    uint16_t read_pt(int);

    // 0 for sprite, 1 for bg, 2 for EXT
    uint8_t pmux[2][2][2] = {
        {{2, 2}, {0, 0}},
        {{1, 1}, {0, 1}}};

    // frame bitmap
    
    vector<array<uint8_t, 3>> frame = vector<array<uint8_t, 3>>(61440, {0,0,0});
    

    //master palette
    vector<array<uint8_t, 3>> mpallette = {{
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
