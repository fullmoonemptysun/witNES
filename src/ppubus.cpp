#include "ppubus.h"
#include <iostream>
using namespace std;

// register operations for the cpu
uint8_t PPUBus::read_register(uint16_t addr)
{
    if (addr >= 0x2000 && addr <= 0x2007)
    {
        switch (addr)
        {
        case 0x2000:
            // debug
            //cout << "CPU: TRYING TO READ FROM: PPUCTRL" << endl;
            return latch;
        case 0x2001:
            //cout << "CPU: TRYING TO READ FROM: PPUMASK" << endl;
            return latch;
        case 0x2002:
            //cout << "CPU: TRYING TO READ FROM: PPUSTATUS" << endl;
            latch = witppu->ppustatus; // latch fills with the data read.
            // reading clears the vblank flag
            witppu->ppustatus &= 0b01111111;
            witppu->wreg = 0x00; // reading PPUSTATUS clears the w register
            return latch;
        case 0x2003:
            //cout << "CPU: TRYING TO READ FROM: OAMADDR" << endl;
            return latch;
        case 0x2004:
            //cout << "CPU: TRYING TO READ FROM: OAMDATA" << endl;
            latch = witppu->oamdata; // fill latch w data
            return witppu->oamdata;
        case 0x2005:
            //cout << "CPU: TRYING TO READ FROM: PPUSCROLL" << endl;
            return latch;
        case 0x2006:
            //cout << "CPU: TRYING TO READ FROM: PPUADDR" << endl;
            return latch;
        case 0x2007:
        {
            //cout << "CPU: TRYING TO READ FROM: PPUDATA" << endl;
            uint8_t val = witppu->pdatabuf; // store old value somewhere

            witppu->ppudata = read_mem(witppu->vreg); // load new data in ppudata and in buf
            witppu->pdatabuf = witppu->ppudata;

            // increment v
            witppu->vreg += (witppu->ppuctrl & 0b00000100) ? 1 : 32; // 1 means go to next tile, 32 means go down a row in the current col
            witppu->vreg = CAST_15(witppu->vreg); // vreg must remain 15 bits wide 


            latch = val;
            return latch;
        }
        default:
            return latch;
        }
    }

    else if (addr >= 0x2008 && addr <= 0x3fff)
    {
        uint16_t tmpaddr = (addr & 0x0007) + 0x2000;
        switch (tmpaddr)
        {
        case 0x2000:
            // debug
            //cout << "CPU: TRYING TO READ FROM: PPUCTRL" << endl;
            return latch;
        case 0x2001:
            //cout << "CPU: TRYING TO READ FROM: PPUMASK" << endl;
            return latch;
        case 0x2002:
            //cout << "CPU: TRYING TO READ FROM: PPUSTATUS" << endl;
            latch = witppu->ppustatus; // latch fills with the data read.
            // reading clears the vblank flag
            witppu->ppustatus &= 0b01111111;
            witppu->wreg = 0x00; // reading PPUSTATUS clears the w register
            return latch;
        case 0x2003:
            //cout << "CPU: TRYING TO READ FROM: OAMADDR" << endl;
            return latch;
        case 0x2004:
            //cout << "CPU: TRYING TO READ FROM: OAMDATA" << endl;
            latch = witppu->oamdata; // fill latch w data
            return witppu->oamdata;
        case 0x2005:
            //cout << "CPU: TRYING TO READ FROM: PPUSCROLL" << endl;
            return latch;
        case 0x2006:
            //cout << "CPU: TRYING TO READ FROM: PPUADDR" << endl;
            return latch;
        case 0x2007:
        {
            //cout << "CPU: TRYING TO READ FROM: PPUDATA" << endl;
            uint8_t val = witppu->pdatabuf; // store old value somewhere

            witppu->ppudata = read_mem(witppu->vreg); // load new data in ppudata and in buf
            witppu->pdatabuf = witppu->ppudata;

            // increment v
            witppu->vreg += (witppu->ppuctrl & 0b00000100) ? 1 : 32; // 1 means go to next tile, 32 means go down a row in the current col

            latch = val;
            return latch;
        }
        default:
            return latch;
        }
    }

    else if (addr == 0x4014)

    {
        //cout << "CPU: TRYING TO READ FROM: OAMDMA" << endl;
        return latch;
    }

    else
    {

        //cout << "READING OUT OF PPU MMIO RANGE!!!" << endl;

        return 0;
    }
}

void PPUBus::write_register(uint16_t addr, uint8_t data, uint16_t cycles)
{
    if (addr >= 0x2000 && addr <= 0x2007)
    {
        switch (addr)
        {

        // PPUCTRL
        case 0x2000:
            if (cycles > 29658)
            {
                //cout << "CPU: TRYING TO WRITE TO: PPUCTRL" << endl;
                witppu->ppuctrl = data;
                witppu->treg = CAST_15((witppu->treg & 0b111001111111111) | (((witppu->ppuctrl) << 10) & 0b000110000000000)); // set nametable bits of treg
                latch = witppu->ppuctrl;
            }

            break;

        // PPUMASK
        case 0x2001:
            if (cycles > 29658)
            {
                //cout << "CPU: TRYING TO WRITE TO: PPUMASK" << endl;
                witppu->ppumask = data;

                // every write to ppumask may change the rendering status
                witppu->bg_render_enable = (bool)(0b00001000 & witppu->ppumask);
                witppu->sp_render_enable = (bool)(0b00010000 & witppu->ppumask);

                witppu->grayscale = (bool)(0x1 & witppu->ppumask); // grayscale mode (0: off, 1: on)

                latch = witppu->ppumask;
            }
            break;

        case 0x2002:
            //cout << "CPU: TRYING TO WRITE TO: PPUSTATUS" << endl;
            latch = data;
            break;

        case 0x2003:
            //cout << "CPU: TRYING TO WRITE TO: OAMADDR" << endl;
            witppu->oamaddr = data;
            latch = witppu->oamaddr;
            break;

        case 0x2004: // OAMDATA

            if ((witppu->scanline >= 0) && (witppu->scanline <= 239) && (witppu->dot >= 1) && (witppu->dot <= 256))
            {
                // no writes to OAM allowed during rendering
                break;
            }

            //cout << "CPU: TRYING TO WRITE TO: OAMDATA" << endl;
            witppu->oamdata = data;
            write_mem(witppu->oamaddr, data);
            latch = witppu->oamdata;
            witppu->oamaddr++;
            break;

        case 0x2005: // PPUSCROLL
            if (cycles > 29658)
            {
                //cout << "CPU: TRYING TO WRITE TO: PPUSCROLL" << endl;

                witppu->ppuscroll = data;
                if (witppu->wreg)
                { // second write Y scroll position
                    witppu->treg = CAST_15(((witppu->treg & 0b000110000011111) | (witppu->ppuscroll << 12)) | ((witppu->ppuscroll & 0b11111000) << 2));
                    witppu->vreg = witppu->treg; // second write, scroll coordinates are loaded so move to v
                }
                else
                {                                                                               // first write X scroll position
                    witppu->treg = CAST_15((witppu->treg & 0xFFE0) | (witppu->ppuscroll >> 3)); // update coarse X
                    witppu->xreg_ppu = (witppu->ppuscroll & 0b00000111);                        // update fine X
                }
                latch = witppu->ppuscroll;

                witppu->wreg ^= 1;
            }
            break;

        case 0x2006: // PPUADDR
            if (cycles > 29658)
            {

                //cout << "CPU: TRYING TO WRITE TO: PPUADDR" << endl;

                witppu->ppuaddr = data;

                if (witppu->wreg)
                { // second write (lower byte)
                    witppu->treg &= 0xFF00;
                    witppu->treg += witppu->ppuaddr;
                    witppu->vreg = witppu->treg; // on second write, address is complete, copy to v.
                }

                else
                { // first write high byte (5-0)
                    witppu->treg &= 0x00FF;
                    witppu->treg = CAST_14(witppu->treg + (witppu->ppuaddr << 8)); // clears bits 14 & 15
                }

                witppu->wreg ^= 1;
                latch = witppu->ppuaddr;
            }
            break;

            // PPUDATA
        case 0x2007:
            //cout << "CPU: TRYING TO WRITE TO: PPUDATA" << endl;

            // only write when not rendering
            if ((!((witppu->ppumask) & 0b00010000) && !((witppu->ppumask) & 0b00001000)) || (witppu->scanline >= 241 && witppu->scanline <= 260))
            {
                witppu->ppudata = data;
                write_mem(witppu->vreg, data); // write to the address in v.
                // increment v
                witppu->vreg += (witppu->ppuctrl & 0b00000100) ? 1 : 32; // 1 means go to next tile, 32 means go down a row in the current col
                witppu->vreg = CAST_15(witppu->vreg); // vreg must remain 15 bits wide 
                latch = witppu->ppudata;
            }

            break;
        }
    }

    else if (addr >= 0x2008 && addr <= 0x3fff)
    {
        uint16_t tmpaddr = (addr & 0x0007) + 0x2000;
        switch (tmpaddr)
        {

        // PPUCTRL
        case 0x2000:
            if (cycles > 29658)
            {
                //cout << "CPU: TRYING TO WRITE TO: PPUCTRL" << endl;
                witppu->ppuctrl = data;
                witppu->treg = CAST_15((witppu->treg & 0b111001111111111) | (((witppu->ppuctrl) << 10) & 0b000110000000000)); // set nametable bits of treg
                latch = witppu->ppuctrl;
            }

            break;

        // PPUMASK
        case 0x2001:
            if (cycles > 29658)
            {
                //cout << "CPU: TRYING TO WRITE TO: PPUMASK" << endl;
                witppu->ppumask = data;

                // every write to ppumask may change the rendering status
                witppu->bg_render_enable = (bool)(0b00001000 & witppu->ppumask);
                witppu->sp_render_enable = (bool)(0b00010000 & witppu->ppumask);

                witppu->grayscale = (bool)(0x1 & witppu->ppumask); // grayscale mode (0: off, 1: on)

                latch = witppu->ppumask;
            }
            break;

        case 0x2002:
            //cout << "CPU: TRYING TO WRITE TO: PPUSTATUS" << endl;
            latch = data;
            break;

        case 0x2003:
            //cout << "CPU: TRYING TO WRITE TO: OAMADDR" << endl;
            witppu->oamaddr = data;
            latch = witppu->oamaddr;
            break;

        case 0x2004: // OAMDATA

            if ((witppu->scanline >= 0) && (witppu->scanline <= 239) && (witppu->dot >= 1) && (witppu->dot <= 256))
            {
                // no writes to OAM allowed during rendering
                break;
            }

            //cout << "CPU: TRYING TO WRITE TO: OAMDATA" << endl;
            witppu->oamdata = data;
            write_mem(witppu->oamaddr, data);
            latch = witppu->oamdata;
            witppu->oamaddr++;
            break;

        case 0x2005: // PPUSCROLL
            if (cycles > 29658)
            {
                //cout << "CPU: TRYING TO WRITE TO: PPUSCROLL" << endl;

                witppu->ppuscroll = data;
                if (witppu->wreg)
                { // second write Y scroll position
                    witppu->treg = CAST_15(((witppu->treg & 0b000110000011111) | (witppu->ppuscroll << 12)) | ((witppu->ppuscroll & 0b11111000) << 2));
                    witppu->vreg = witppu->treg; // second write, scroll coordinates are loaded so move to v
                }
                else
                {                                                                               // first write X scroll position
                    witppu->treg = CAST_15((witppu->treg & 0xFFE0) | (witppu->ppuscroll >> 3)); // update coarse X
                    witppu->xreg_ppu = (witppu->ppuscroll & 0b00000111);                        // update fine X
                }
                latch = witppu->ppuscroll;

                witppu->wreg ^= 1;
            }
            break;

        case 0x2006: // PPUADDR
            if (cycles > 29658)
            {

                //cout << "CPU: TRYING TO WRITE TO: PPUADDR" << endl;

                witppu->ppuaddr = data;

                if (witppu->wreg)
                { // second write (lower byte)
                    witppu->treg &= 0xFF00;
                    witppu->treg += witppu->ppuaddr;
                    witppu->vreg = witppu->treg; // on second write, address is complete, copy to v.
                }

                else
                { // first write high byte (5-0)
                    witppu->treg &= 0x00FF;
                    witppu->treg = CAST_14(witppu->treg + (witppu->ppuaddr << 8)); // clears bits 14 & 15
                }

                witppu->wreg ^= 1;
                latch = witppu->ppuaddr;
            }
            break;

            // PPUDATA
        case 0x2007:
            //cout << "CPU: TRYING TO WRITE TO: PPUDATA" << endl;

            // only write when not rendering
            if ((!((witppu->ppumask) & 0b00010000) && !((witppu->ppumask) & 0b00001000)) || (witppu->scanline >= 241 && witppu->scanline <= 260))
            {
                witppu->ppudata = data;
                write_mem(witppu->vreg, data); // write to the address in v.
                // increment v
                witppu->vreg += (witppu->ppuctrl & 0b00000100) ? 1 : 32; // 1 means go to next tile, 32 means go down a row in the current col

                latch = witppu->ppudata;
            }

            break;
        }
    }

    else if (addr == 0x4014)
    {
        //cout << "[INFO] OAMDMA HAPPENED" << endl;
    }

    else
    {

        //cout << "WRITING OUT OF PPU MMIO RANGE!!!" << endl;
    }
}

// memory operations (PPU <-> MEMORY)

uint8_t PPUBus::read_mem(uint16_t add)
{


    uint16_t addr = CAST_14(add);
    // nametable
    if (addr >= 0x2000 && addr <= 0x2fff)
    {

        // horizontal mirroring
        if (this->mainbus->cart->mirroring)
        {
            if ((addr >= 0x2800))
            {
                return vram[addr - 0x800];
            }

            else
            {
                return vram[addr];
            }
        }

        else
        {
            // vertical mirroring
            if ((addr >= 0x2400 && addr < 0x2c00))
            {
                return vram[addr - 0x400];
            }

            else if (addr >= 0x2c00)
            {
                return vram[addr - 0x800];
            }

            else
            {
                return vram[addr];
            }
        }
    }

    // pattern table
    else if (addr >= 0x0000 && addr <= 0x1fff)
    {
        return mainbus->cart->ppu_read(addr);
    }

    else if (addr >= 0x3000 && addr <= 0x3eff)
    {
        //cout << "READING FROM UNUSED MEMORY" << endl;
        return read_mem((addr - 0x1000));
    }

    // pallette mirror
    else if (addr >= 0x3f20 && addr <= 0x3fff)
    {
        return read_mem((addr & 0x1F) + 0x3f00);
    }

    // pallette
    else if (addr >= 0x3f00 && addr <= 0x3f1f)
    {
        // 0th entry of each palette is mirrored for background and sprites.
        switch (addr)
        {
        case (0x3f10):
            return pallette[(addr - 0x10) - 0x3f00];

        case (0x3f14):
            return pallette[(addr - 0x10) - 0x3f00];

        case (0x3f18):
            return pallette[(addr - 0x10) - 0x3f00];

        case (0x3f1c):
            return pallette[(addr - 0x10) - 0x3f00];
        }
        return pallette[addr - 0x3f00];
    }

    else
    {
        //cout << "ERROR: TRYING TO READ OUT OF PPU MEMORY SPACE" << endl;

        exit(-1);
    }
}

void PPUBus::write_mem(uint16_t add, uint8_t data)
{

    uint16_t addr = CAST_14(add); //ensure the address is wrapped around to 14 bits
    // vram
    if (addr >= 0x2000 && addr <= 0x2fff)
    {

        // horizontal mirroring
        if (this->mainbus->cart->mirroring)
        {

            // debug
            //cout << "HORIZONTAL MIRRORING DETECTED\n";
            if ((addr >= 0x2800))
            {
                vram[addr - 0x800] = data;
            }

            else
            {
                vram[addr] = data;
            }
        }

        else
        {
            // vertical mirroring
            // debug
            //cout << "VERTICAL MIRRORING\n";
            if ((addr >= 0x2400 && addr < 0x2c00))
            {
                vram[addr - 0x400] = data;
            }

            else if (addr >= 0x2c00)
            {
                vram[addr - 0x800] = data;
            }

            else
            {
                vram[addr] = data;
            }
        }
    }

    else if (addr >= 0x3f00 && addr <= 0x3f1f)
    {
        // 0th entry of each palette is mirrored for background and sprites.
        switch (addr)
        {
        case (0x3f10):
            pallette[(addr - 0x10) - 0x3f00] = data;
            break;

        case (0x3f14):
            pallette[(addr - 0x10) - 0x3f00] = data;
            break;

        case (0x3f18):
            pallette[(addr - 0x10) - 0x3f00] = data;
            break;

        case (0x3f1c):
            pallette[(addr - 0x10) - 0x3f00] = data;
            break;

        default:
            pallette[addr - 0x3f00] = data;
        }
    }

        // pattern table
    else if (addr >= 0x0000 && addr <= 0x1fff)
    {
        //chr-rom read only.
    }
    // pallette mirror
    else if (addr >= 0x3f20 && addr <= 0x3fff)
    {
        write_mem(((addr & 0x1F) + 0x3f00), data);
    }

    else if (addr >= 0x3000 && addr <= 0x3eff)
    {
        //cout << "READING FROM UNUSED MEMORY" << endl;
        write_mem((addr - 0x1000), data);
    }

    else
    {
        //cout << "ERROR: TRYING TO WRITE OUTSIDE OF PPU ADDR SPACE!!!" << endl;
        exit(-1);
    }
}
