#include "ppu.h"
#include <iostream>
#include "disasm.h"

#define CAST_15(x) ((x) & 0x7FFF)
#define CAST_14(x) ((x) & 0x3fff)

using namespace std;

uint8_t ppu::read_nt(int index)
{

    uint8_t base_nt_addr = (vreg & 0xc00) >> 10;

    if (bus->mainbus->cart->mirroring)
    {
        // horizontal mirroring

        switch (base_nt_addr)
        {
        case 0:
            return read(0x2000 + index);
        case 1:
            return read(0x2400 + index);
        case 2: // 0x2800
            return read(0x2000 + index);

        case 3: // 0x2c00
            return read(0x2400 + index);
        }
    }

    else
    {
        // vertical mirroring
        switch (base_nt_addr)
        {
        case 0:
            return read(0x2000 + index);
        case 1: // 0x2400
            return read(0x2000 + index);
        case 2: // 0x2800
            return read(0x2400 + index);

        case 3: // 0x2c00
            return read(0x2400 + index);
        }
    }
}

uint8_t ppu::read_at(int index)
{

    uint8_t base_nt_addr = (vreg & 0xc00) >> 10;

    if (bus->mainbus->cart->mirroring)
    {
        // horizontal mirroring

        switch (base_nt_addr)
        {
        case 0:
            return read(0x23c0 + index);
        case 1:
            return read(0x27c0 + index);
        case 2: // 0x2800
            return read(0x23c0 + index);

        case 3: // 0x2c00
            return read(0x27c0 + index);
        }
    }

    else
    {
        // vertical mirroring
        switch (base_nt_addr)
        {
        case 0:
            return read(0x23c0 + index);
        case 1: // 0x2400
            return read(0x23c0 + index);
        case 2: // 0x2800
            return read(0x27c0 + index);

        case 3: // 0x2c00
            return read(0x27c0 + index);
        }
    }
}
uint16_t ppu::read_pt(int tileno)
{
    // tileno * 16 + vertical pixel offset + plane * 8
    uint16_t pt_addr = tileno * 16 + ((vreg & 0x7000) >> 12) + 0 * 8; // address for left pt lobyte
    uint8_t pt_base_addr = ppuctrl & 0b00010000;                      // 0 means $0000 non 0 means $1000

    if (pt_base_addr)
    {
        pt_addr |= 0x1000;
        uint8_t lobyte = bus->read_mem(pt_addr);
        uint8_t hibyte = bus->read_mem(pt_addr + 8);

        return ((hibyte << 8) + lobyte);
    }

    else
    {
        uint8_t lobyte = bus->read_mem(pt_addr);
        uint8_t hibyte = bus->read_mem(pt_addr + 8);

        return ((hibyte << 8) + lobyte);
    }
}
void ppu::clock()
{

    // idle cycle
    if ((scanline >= 0 && scanline <= 239) && dot == 0)
    {
        dot += 1;
    }

    // VISUAL DOTS AND SCANLINES
    else if ((scanline >= 0 && scanline <= 239) && (1 <= dot && dot <= 256) && (sp_render_enable || bg_render_enable))
    {
        // render current dot first

        // if 8 dot boundary, then do mem fetches
        if ((dot % 8 == 0))
        {

            //inc. hor(v)
            int coarse_x = vreg & 0x1F;
            //out of nametable boundary horizontally
            if((coarse_x + 1) >= 32){
                vreg = vreg & 0xFFE0;
                //flip horizontal bit
                vreg ^= 0x0400; // flip bit 10
            }
            else{
                vreg = CAST_15(vreg+1);
            }

            //nt fetch
            // get tile no. from nametable
            uint8_t tileno = read_nt(vreg & 0x3ff);
            uint16_t ptdata = read_pt(tileno);

            // Set the shift registers next 8 bits
            shft_reg_hi |= (ptdata >> 8);
            shft_reg_lo |= (ptdata & 0x00ff);

            // debugging
            cout << "0x" << toHex(tileno) << endl;

            //at fetch
            uint8_t at_byte = read_at((vreg >> 4) & 0x38) | ((vreg >> 2) & 0x07);
            uint8_t val =  ((vreg & 0x001F) & 0x2)  + ((((vreg & 0x1E0) >> 5) & 0x2)>>1);
            switch (val){
                case 0: // top-left
                    attr_reg_hi |= ((at_byte & 0b00000010) >> 1) * 0xFF;
                    attr_reg_lo |= ((at_byte & 0b00000001)) * 0xFF;
                    break;
                case 1:
                    attr_reg_hi |= ((at_byte & 0b00100000) >> 5) * 0xFF;
                    attr_reg_lo |= ((at_byte & 0b00010000) >> 4) * 0xFF;
                    break;
                case 2:
                    attr_reg_hi |= ((at_byte & 0b00001000) >> 3) * 0xFF;
                    attr_reg_lo |= ((at_byte & 0b00000100) >> 2) * 0xFF;
                    break;
                case 3:
                    attr_reg_hi |= ((at_byte & 0b10000000) >> 7) * 0xFF;
                    attr_reg_lo |= ((at_byte & 0b01000000) >> 6) * 0xFF;
                    break;
                default:
                //debug
                    cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
            }




        }



    }
}
