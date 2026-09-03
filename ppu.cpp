/**
 * @author Vinnie, 2026
 * If you wish to use this implementation of the NES PPU, please give credit to the author wherever due.
 *
 *
 *
 * TODO: 09/01/2026 Start the rest of the cycles (vblank polling, pre render, post render scanlines), add sprite 0 hit update logic if it occurs.
 */

#include "ppu.h"
#include <iostream>
#include "disasm.h"

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
    }

    uint8_t lobyte = bus->read_mem(pt_addr);
    uint8_t hibyte = bus->read_mem(pt_addr + 8);

    return ((hibyte << 8) + lobyte);
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

        // calculate bgpx
        int index = xreg_ppu;
        uint8_t bgpxaddr = (((shft_reg_hi >> (6 - index)) & 0x2) | (shft_reg_lo >> (7 - index))) | (((attr_reg_hi >> (4 - index)) & 0x8) | ((attr_reg_lo >> (5 - index)) & 0x4));

        // calculate sprite pixel
        uint8_t spxaddr = 0;

        // TODO: Add the 5th bit for sprites
        // TODO: Check bg_render_enable and sp_render_enable — treat disabled component as transparent (pixel = 0) before priority mux
        // decide which to render

        // TODO: SET SPRITE 0 HIT FLAG

        // FOR NOW EITHER BG or EXT. TODO: FIX THIS AFTER SPRITE PART IS DONE.
        uint8_t priority_mux_val = pmux[((bgpxaddr & 0x3) != 0)][0x00][0x00]; //[HARDCODED]

        switch (priority_mux_val)
        {
        case (0):
            break;
        case (1):
            uint8_t curr_px_idx = bus->read_mem(0x3f00 + bgpxaddr); // 6 bit
            // sdl2 rendering pipeline.
            // grab the rgb from master pallette
            frame[scanline * 256 + (dot - 1)] = mpallette[CAST_6(curr_px_idx)];

            break;
        case (2):
            frame[scanline * 256 + (dot - 1)] = mpallette[bus->read_mem(0x3f00)]; // EXT color
            break;
        }

        // if 8 dot boundary, then do mem fetches
        if ((dot % 8 == 0) && dot < 256)
        {

            // inc. hor(v)
            int coarse_x = vreg & 0x1F;
            // out of nametable boundary horizontally
            if ((coarse_x + 1) >= 32)
            {
                vreg = vreg & 0xFFE0;
                // flip horizontal bit
                vreg ^= 0x0400; // flip bit 10
            }
            else
            {
                vreg = CAST_15(vreg + 1);
            }

            // nt fetch
            //  get tile no. from nametable
            uint8_t tileno = read_nt(vreg & 0x3ff);
            uint16_t ptdata = read_pt(tileno);

            // Set the shift registers next 8 bits
            shft_reg_hi |= (ptdata >> 8);
            shft_reg_lo |= (ptdata & 0x00ff);

            // debugging
            cout << "0x" << toHex(tileno) << endl;

            // at fetch
            uint8_t at_byte = read_at(((vreg >> 4) & 0x38) | ((vreg >> 2) & 0x07));
            uint8_t val = ((vreg & 0x001F) & 0x2) + ((((vreg & 0x1E0) >> 5) & 0x2) >> 1);
            switch (val)
            {
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
                // debug
                cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
            }
        }

        // at dot 256
        else
        {
            // inc. fine_y
            uint8_t fine_y = ((vreg >> 12) & 0x0F);

            fine_y += 1;
            if (fine_y > 7)
            {
                vreg &= 0x0fff; // clear fine_y
                uint8_t coarse_y = (vreg >> 5) & 0b0011111;
                vreg &= 0xfc1f; // clear coarse_y

                coarse_y += 1;

                if (coarse_y > 29)
                {
                    vreg ^= 0x0800; // flip vertical bit
                }

                else
                {
                    vreg += (coarse_y << 5);
                }
            }
            else
            {
                vreg &= 0x0fff; // clear fine_y
                vreg += fine_y << 12;
            }
        }
    }

    // Dots after the visible cycles (visible scanline) (257 - 320)
    else if ((scanline >= 0 && scanline <= 239) && (257 <= dot && dot <= 320))
    {
        // oamaddr is cleared

        // TODO: IMPLEMENT SPRITE TILE DATA FETCH WHEN READY FOR SPRITE RENDERING
        oamaddr = 0x00;

        // return the horizontal n table bit and coarse_x to v from t. (Horizontal reset)
        vreg = CAST_15((vreg & 0xFBE0) | (treg & 0x041f)); // clear namtable bit and coarse_x
    }

    else if ((scanline >= 0 && scanline <= 239) && (321 <= dot && dot <= 336))
    {
        // do 1st set of fetches here
        if (dot == 321)
        {

            // nt fetch
            //  get tile no. from nametable
            uint8_t tileno = read_nt(vreg & 0x3ff);
            uint16_t ptdata = read_pt(tileno);

            // Set the shift registers next 8 bits (on the upper byte this time)
            shft_reg_hi = (ptdata & 0xff00);
            shft_reg_lo = ((ptdata & 0x00ff) << 8);

            // debugging
            cout << "0x" << toHex(tileno) << endl;

            // at fetch
            uint8_t at_byte = read_at(((vreg >> 4) & 0x38) | ((vreg >> 2) & 0x07));
            uint8_t val = ((vreg & 0x001F) & 0x2) + ((((vreg & 0x1E0) >> 5) & 0x2) >> 1);
            switch (val)
            {
            case 0: // top-left
                attr_reg_hi = (((at_byte & 0b00000010) >> 1) * 0xFF) << 8;
                attr_reg_lo = (((at_byte & 0b00000001)) * 0xFF) << 8;
                break;
            case 1:
                attr_reg_hi = (((at_byte & 0b00100000) >> 5) * 0xFF) << 8;
                attr_reg_lo = (((at_byte & 0b00010000) >> 4) * 0xFF) << 8;
                break;
            case 2:
                attr_reg_hi = (((at_byte & 0b00001000) >> 3) * 0xFF) << 8;
                attr_reg_lo = (((at_byte & 0b00000100) >> 2) * 0xFF) << 8;
                break;
            case 3:
                attr_reg_hi = (((at_byte & 0b10000000) >> 7) * 0xFF) << 8;
                attr_reg_lo = (((at_byte & 0b01000000) >> 6) * 0xFF) << 8;
                break;
            default:
                // debug
                cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
            }
        }

        // second set of fetches
        else if (dot == 323)
        {
            // inc. hor(v)
            int coarse_x = vreg & 0x1F;
            // out of nametable boundary horizontally
            if ((coarse_x + 1) >= 32)
            {
                vreg = vreg & 0xFFE0;
                // flip horizontal bit
                vreg ^= 0x0400; // flip bit 10
            }
            else
            {
                vreg = CAST_15(vreg + 1);
            }

            // nt fetch
            //  get tile no. from nametable
            uint8_t tileno = read_nt(vreg & 0x3ff);
            uint16_t ptdata = read_pt(tileno);

            // Set the shift registers next 8 bits
            shft_reg_hi |= (ptdata >> 8);
            shft_reg_lo |= (ptdata & 0x00ff);

            // debugging
            cout << "0x" << toHex(tileno) << endl;

            // at fetch
            uint8_t at_byte = read_at(((vreg >> 4) & 0x38) | ((vreg >> 2) & 0x07));
            uint8_t val = ((vreg & 0x001F) & 0x2) + ((((vreg & 0x1E0) >> 5) & 0x2) >> 1);
            switch (val)
            {
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
                // debug
                cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
            }
        }

        // rest of the dots are idle
    }
}
