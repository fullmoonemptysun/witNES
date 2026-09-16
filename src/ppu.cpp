/**
 * @author Vinnie, 2026
 * If you wish to use this implementation of the NES PPU, please give credit to the author wherever due.
 *
 *
 *
 * TODO: 09/06/2026 rendering flow with SDL2(bg only), add sprite 0 hit update logic if it occurs.
 */

#include "ppu.h"
#include <iostream>
#include "disasm.h"
#include "ppubus.h"

using namespace std;

void ppu::reset()
{
    // debug
    cout << "RESETTING PPU" << endl;

    ppuctrl = 0;
    ppustatus = 0;
    ppumask = 0;
    ppudata = 0;
    ppuaddr = 0;
    ppuscroll = 0;
    oamaddr = 0;
    oamdata = 0;
    oamdma = 0;

    scanline = 0;
    dot = 0;
}

// ppubus read/write
uint8_t ppu::read(uint16_t addr)
{

    // debug
    cout << "PPU READING FROM MEM addr: " << toHex((uint16_t)addr) << '\n';
    return bus->read_mem(addr);
}

void ppu::write(uint16_t addr, uint8_t data)
{
    // debug
    cout << "PPU Writing to MEM addr: " << toHex((uint16_t)addr) << ", ";
    // debug
    cout << "PPU Writing to MEM data: " << toHex((uint8_t)data) << '\n';
    bus->write_mem(addr, data);
}

uint8_t ppu::read_nt(int index)
{

    cout << "PPU READING NAMETABLE" << endl;

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

        default:
            std::cout << "ERROR: READ_NT" << '\n';
            return -1;
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

        default:
            std::cout << "ERROR: READ_NT" << '\n';
            return -1;
        }
    }
}

uint8_t ppu::read_at(int index)
{

    cout << "PPU READING ATTR_TABLE" << endl;

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

        default:
            std::cout << "ERROR: READ_at" << '\n';
            return -1;
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

        default:
            std::cout << "ERROR: READ_aT" << '\n';
            return -1;
        }
    }
}
uint16_t ppu::read_pt(int tileno)
{
    cout << "PPU READING PTABLE" << endl;
    // tileno * 16 + vertical pixel offset + plane * 8
    uint16_t pt_addr = tileno * 16 + ((vreg & 0x7000) >> 12) + 0 * 8; // address for left pt lobyte
    uint8_t pt_base_addr = ppuctrl & 0b00010000;                      // 0 means $0000 non 0 means $1000

    if (pt_base_addr)
    {
        pt_addr |= 0x1000;
    }

    uint8_t lobyte = read(pt_addr);
    uint8_t hibyte = read(pt_addr + 8);

    return ((hibyte << 8) + lobyte);
}
void ppu::clock()
{

    // debug
    std::cout << "PPU_TICK: ["<<"SC:" << scanline << " DOT: " << dot << " ]" << '\n';
    std::cout << "REGS: " "[" << " PPUCTRL:" << toHex(ppuctrl) << " |" << " PPUSTATUS:" << toHex(ppustatus) << " |" << " PPUMASK:" << toHex(ppumask) << " |" << " PPUSCROLL:" << toHex(ppuscroll) << " |" << " PPUADDR:" << toHex(ppuaddr) << " |" << " PPUDATA:" << toHex(ppudata) << " |" << " OAMADDR:" << toHex(oamaddr) << " |" <<   "]" << "\n\n" << endl;
    // debug
    

    // idle cycle
    if ((scanline >= 0 && scanline <= 239) && dot == 0)
    {
        // debug
        cout << "PPU: ENTERED DOT 1 of RENDERING(IDLE CYCLE)" << '\n';
        dot += 1;
    }

    // if rendering is not enabled.
    else if ((scanline >= 0 && scanline <= 239) && (1 <= dot && dot <= 256))
    {
        // debug
        cout << "PPU: ENTERED RENDERING PHASE (DISABLED)" << endl;
        dot += 1;
    }

    // VISUAL DOTS AND SCANLINES
    else if ((scanline >= 0 && scanline <= 239) && (1 <= dot && dot <= 256) && (sp_render_enable || bg_render_enable))
    {

        // debug
        cout << "PPU: ENTERED RENDERING PHASE" << '\n';
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
        {
            // debug
            cout << "PPU: RENDERING BG for CURRENT PX (RGB: ";
            uint8_t curr_px_idx = read(0x3f00 + bgpxaddr); // 6 bit
            // sdl2 rendering pipeline.
            // grab the rgb from master pallette
            frame[scanline * 256 + (dot - 1)] = mpallette[CAST_6(curr_px_idx)];

            // debug
            for (int i = 0; i < 3; i++)
            {
                cout << "0x" << toHex((uint8_t)frame[scanline * 256 + (dot - 1)][i]) << ", ";
            }
            cout << endl;

            break;
        }
        case (2):
            // debug
            cout << "PPU: RENDERING EXT for CURRENT PX (RGB: ";
            frame[scanline * 256 + (dot - 1)] = mpallette[read(0x3f00)]; // EXT color

            // debug
            for (int i = 0; i < 3; i++)
            {
                cout << "0x" << toHex((uint8_t)frame[scanline * 256 + (dot - 1)][i]) << ", ";
            }
            cout << endl;
            break;
        }

        // if 8 dot boundary, then do mem fetches
        if ((dot % 8 == 0) && dot < 256)
        {

            // debug
            cout << "AT 8 DOT BOUNDARY DURING RENDERING. FETCHING VALUES..." << '\n';
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
            std::cout << "0x" << toHex(tileno) << endl;

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
                std::cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
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

        dot += 1;
    }

    // Dots after the visible cycles (visible scanline) (257 - 320)
    else if ((scanline >= 0 && scanline <= 239) && (257 <= dot && dot <= 320))
    {

        // debug
        cout << "ENTERED POST RENDER CYCLE - RENDERING PHASE (>256)" << endl;

        // debug
        if (dot == 257 && RENDERING_ENABLE)
        {

            // oamaddr is cleared
            // TODO: IMPLEMENT SPRITE TILE DATA FETCH WHEN READY FOR SPRITE RENDERING
            oamaddr = 0x00;

            // return the horizontal n table bit and coarse_x to v from t. (Horizontal reset)
            vreg = CAST_15((vreg & 0xFBE0) | (treg & 0x041f)); // clear namtable bit and coarse_x
            cout << "HORIZONTAL V RESTORED" << endl;
        }

        dot += 1;
    }

    else if ((scanline >= 0 && scanline <= 239) && (321 <= dot && dot <= 336))
    {

        if ((sp_render_enable || bg_render_enable))
        {
            cout << "ENTERED POST RENDER CYCLES - DOING 2 FETCHES FOR NEXT SCANLINE" << endl;

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
                std::cout << "0x" << toHex(tileno) << endl;

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
                    std::cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
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
                std::cout << "0x" << toHex(tileno) << endl;

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
                    std::cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
                }
            }
        }

        else
        {
            // debug
            cout << "ENTERED POST RENDER CYCLES - NOFETCH (R_DISABLED)" << endl;
            // do 1st set of fetches here
        }
        // rest of the dots are idle
        dot += 1;
    }

    else if ((scanline >= 0 && scanline <= 239) && (337 <= dot && dot <= 339))
    {
        // debug
        cout << "ENTERED GARBAGE POST RENDER CYCLES 337-340" << endl;
        dot += 1;
    }

    else if ((scanline <= 260) && dot == 340)
    {

        // debug
        cout << "ENTERED LAST CYCLE NOT THE END" << endl;

        // reset rendering variables
        dot = 0;
        scanline += 1;
    }

    // pre render scanline
    else if (scanline == 261)
    {

        // debug
        cout << "ENTERED PRERENDER SCANLINE (261) - ";

        if (dot < 340)
        {

            // debug
            cout << " NOT THE LAST CYCLE" << endl;
            // random dot chosen to update state for next frame
            if (dot == 280 && RENDERING_ENABLE)
            {

                // debug
                cout << "DOING FIRST FETCH FOR THE NEXT FRAME" << endl;
                vreg = treg; // put scroll value in v

                // nt fetch
                //  get tile no. from nametable
                uint8_t tileno = read_nt(vreg & 0x3ff);
                uint16_t ptdata = read_pt(tileno);

                // Set the shift registers next 8 bits (on the upper byte this time)
                shft_reg_hi = (ptdata & 0xff00);
                shft_reg_lo = ((ptdata & 0x00ff) << 8);

                // debugging
                std::cout << "0x" << toHex(tileno) << endl;

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
                    std::cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
                }
            }

            // another random dot to do 2nd set of fetches
            else if (dot == 285 && RENDERING_ENABLE)
            {

                // debug
                cout << "DOING 2nd FETCHES for THE NEXT SCANLINE" << endl;
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
                std::cout << "0x" << toHex(tileno) << endl;

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
                    std::cout << "ERROR in SELECTING ATTRIBUTE CORNER" << endl;
                }
            }
            dot += 1;
        }

        else
        {
            // debug
            cout << " LAST CYCLE (FRAME RESET)" << endl;
            // reset rendering variables
            dot = 0;
            scanline = 0;
        }
    }

    // post render scanline is idle (240)
    // it is guaranteed the dot will be < 340 if the program enters this block
    else if (scanline == 240)
    {

        // debug
        cout << "ENTERED POSTSCANLINE (SKIPPED)" << endl;

        if (dot == 2)
        {
            // debug
            cout << "PUSHING FRAME CURRENT FRAME OUT" << endl;
            this->screen->refresh_frame(frame);
        }

        dot += 1;
    }

    // vBlank
    else if (scanline >= 241 && scanline <= 260)
    {

        //debug
        cout << "ENTERED VBLANK" << endl;


        // check if vblank enbabled

        if (scanline == 241 && dot == 1)
        {
            ppustatus |= 0b10000000; // set vblank flag
            //debug 
            cout << "VBLANK FLAG SET IN PPUSTATUS";

            if ((ppuctrl & 0b10000000))
            {

                //debug
                cout << "NMI ENABLED!: FIRE" << endl;
                bus->mainbus->witcpu->nmi_due = true;
            }
        }

        else if (scanline == 260 && dot == 1)
        {
            ppustatus &= 0b01111111;
        }

        dot += 1;
    }
}
