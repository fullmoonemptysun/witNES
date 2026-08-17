#include "ppubus.h"
#include <iostream>
using namespace std;

// register operations for the cpu
uint8_t PPUBus::read_register(uint16_t addr)
{
    if (addr >= 0x2000 && addr <= 2007)
    {
        switch (addr)
        {
        case 0x2000:
            return latch;
        case 0x2001:
            return latch;
        case 0x2002:
            latch = witppu->ppustatus; // latch fills with the data read.
            // reading clears the vblank flag
            witppu->ppustatus &= 0b01111111;
            return witppu->ppustatus;
        case 0x2003:
            return latch;
        case 0x2004:
            latch = witppu->oamdata; // fill latch w data
            return witppu->oamdata;
        case 0x2005:

            return latch;
        case 0x2006:
            return latch;
        case 0x2007:
            latch = witppu->ppudata; // fill latch with data
            return witppu->ppudata;
        default:
            return 0;
        }
    }

    else if (addr >= 0x2008 && addr <= 0x3fff)
    {
        uint16_t tmpaddr = (addr & 0x0007) + 0x2000;
        switch (tmpaddr)
        {
        case 0x2000:
            return latch;
        case 0x2001:
            return latch;
        case 0x2002:
            latch = witppu->ppustatus; // latch fills with the data read.
            witppu->wreg = 0x00; //reading PPUSTATUS clears the w register
            return witppu->ppustatus;
        case 0x2003:
            return latch;
        case 0x2004:
            latch = witppu->oamdata; // fill latch w data
            // TODO: Reads during vertical or forced blanking return the value from OAM at that address.

            return witppu->oamdata;
        case 0x2005:

            return latch;
        case 0x2006:
            return latch;
        case 0x2007:
            latch = witppu->ppudata; // fill latch with data
            return witppu->ppudata;

        default:
            return;
        }
    }

    else if (addr == 0x4014)

    {

        return latch;
    }

    else
    {

        cout << "READING OUT OF PPU MMIO RANGE!!!" << endl;

        return 0;
    }
}

void PPUBus::write_register(uint16_t addr, uint8_t data, uint16_t cycles)
{
    if (addr >= 0x2000 && addr <= 2007)
    {
        switch (addr)
        {

        // PPUCTRL
        case 0x2000:
            if (cycles > 29658)
            {
                witppu->ppuctrl = data;
                witppu->treg = CAST_15((witppu->treg & 0b111001111111111) | (((witppu->ppuctrl) << 10) & 0b000110000000000)); //set nametable bits of treg
                latch = witppu->ppuctrl;
            }

            break;

        // PPUMASK
        case 0x2001:
            if (cycles > 29658)
            {
                witppu->ppumask = data;

                // every write to ppumask may change the rendering status
                witppu->bg_render_enable = (bool)(0b00001000 & witppu->ppumask);
                witppu->sp_render_enable = (bool)(0b00010000 & witppu->ppumask);

                witppu->grayscale = (bool)(0x1 & witppu->ppumask); // grayscale mode (0: off, 1: on)

                latch = witppu->ppumask;
            }
            break;

        case 0x2002:
            latch = data;
            break;

        case 0x2003:
            witppu->oamaddr = data;
            latch = witppu->oamaddr;
            break;

        case 0x2004:

            if((witppu->scanline >= 0) && (witppu->scanline <= 239) && (witppu->dot >= 1) && (witppu->dot <= 256)){
                //no writes to OAM allowed during rendering
                break;
            }
            witppu->oamdata = data;
            latch = witppu->oamdata;
            witppu->oamaddr++;
            break;

        case 0x2005:
            if (cycles > 29658)
            {
                
                witppu->ppuscroll = data;
                if(witppu->wreg){//second write Y scroll position
                    witppu->treg = CAST_15(((witppu->treg & 0b000110000011111) | (witppu->ppuscroll << 12)) | ((witppu->ppuscroll & 0b11111000) << 2));
                }
                else{//first write X scroll position
                    witppu->treg = CAST_15((witppu->treg & 0xFFE0) | (witppu->ppuscroll >> 3)); //update coarse X
                    witppu->xreg_ppu = (witppu->ppuscroll & 0b00000111); //update fine X
                    
                }
                latch = witppu->ppuscroll;
            }
            break;

        case 0x2006:
            if (cycles > 29658)
            {
                witppu->ppuaddr = data;
                latch = witppu->ppuaddr;
            }
            break;
        case 0x2007:
            witppu->ppudata = data;
            latch = witppu->ppudata;
            break;
        }
    }

    else if (addr >= 0x2008 && addr <= 0x3fff)
    {
        uint16_t tmpaddr = (addr & 0x0007) + 0x2000;
        switch (tmpaddr)
        {
        case 0x2000:

            witppu->ppuctrl = data;
            latch = witppu->ppuctrl;
            break;

        case 0x2001:
            witppu->ppumask = data;
            latch = witppu->ppumask;
            break;

        case 0x2002:
            latch = data;
            break;

        case 0x2003:
            witppu->oamaddr = data;
            latch = witppu->oamaddr;
            break;

        case 0x2004:
            witppu->oamdata = data;
            latch = witppu->oamdata;
            witppu->oamaddr += 1;
            break;

        case 0x2005:
            witppu->ppuscroll = data;
            latch = witppu->ppuscroll;
            break;

        case 0x2006:
            witppu->ppuaddr = data;
            latch = witppu->ppuaddr;
            break;
        case 0x2007:
            witppu->ppudata = data;
            latch = witppu->ppudata;
            break;
        }
    }

    else if (addr == 0x4014)
    {
    }

    else
    {

        cout << "WRITING OUT OF PPU MMIO RANGE!!!" << endl;
    }
}

// memory operations (PPU <-> MEMORY)

uint8_t PPUBus ::read_mem(uint16_t addr)
{
    // nametable
    if (addr >= 0x2000 && addr <= 0x27ff)
    {
        return vram[addr - 0x2000];
    }

    // pattern table
    else if (addr >= 0x0000 && addr <= 0x1fff)
    {
        return mainbus->cart->ppu_read(addr);
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
}

void PPUBus ::write_mem(uint16_t addr, uint8_t data)
{
    // vram
    if (addr >= 0x2000 && addr <= 0x27ff)
    {
        vram[addr - 0x2000] = data;
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

    // pallette mirror
    else if (addr >= 0x3f20 && addr <= 0x3fff)
    {
        write_mem(((addr & 0x1F) + 0x3f00), data);
    }
}
