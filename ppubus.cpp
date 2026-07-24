#include "ppubus.h"
#include <iostream>
using namespace std;


//register operations for the cpu
uint8_t PPUBus::read_register(uint16_t addr){
    if(addr >= 0x2000 && addr <= 2007){
        switch(addr){
            case 0x2000:
                return latch;
            case 0x2001:
                return latch;
            case 0x2002:
                latch = witppu -> ppustatus; //latch fills with the data read.
                return witppu->ppustatus;
            case 0x2003:
                return latch;
            case 0x2004:
                latch = witppu->oamdata; //fill latch w data
                return witppu->oamdata;
            case 0x2005:

                return latch;      
            case 0x2006:
                return latch;    
            case 0x2007:
                latch = witppu->ppudata;//fill latch with data
                return witppu->ppudata;
            default:
                return 0;
            
        }
    }

    else if(addr >= 0x2008 && addr <= 0x3fff){
        uint16_t tmpaddr = (addr & 0x0007) + 0x2000;
        switch(tmpaddr){
            case 0x2000:
                return latch;
            case 0x2001:
                return latch;
            case 0x2002:
                latch = witppu -> ppustatus; //latch fills with the data read.
                return witppu->ppustatus;
            case 0x2003:
                return latch;
            case 0x2004:
                latch = witppu->oamdata; //fill latch w data
                return witppu->oamdata;
            case 0x2005:

                return latch;      
            case 0x2006:
                return latch;    
            case 0x2007:
                latch = witppu->ppudata;//fill latch with data
                return witppu->ppudata;
            
            default:
                return;
            
        }

    }


    else if(addr == 0x4014){
        return latch;
    }


    else{
        
        cout << "READING OUT OF PPU MMIO RANGE!!!" << endl;

        return 0;
    }
}

void PPUBus::write_register(uint16_t addr, uint8_t data, uint16_t cycles){
    if(addr >= 0x2000 && addr <= 2007){
        switch(addr){

            //PPUCTRL
            case 0x2000:
                if(cycles > 29658){
                    witppu->ppuctrl = data;
                    latch = witppu->ppuctrl;
                }

                break;


            //PPUMASK
            case 0x2001:
                if(cycles > 29658){
                witppu->ppumask = data;

                //every write to ppumask may change the rendering status
                witppu->bg_render_enable = (bool)(0b00001000 & witppu->ppumask);
                witppu->sp_render_enable = (bool)(0b00010000 & witppu->ppumask);
                
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
                witppu->oamdata = data;
                latch = witppu->oamdata;
                break;

            case 0x2005:
                if(cycles > 29658){
                witppu->ppuscroll = data;  
                latch = witppu->ppuscroll;
                }
                break;

            case 0x2006:
                if(cycles > 29658){
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

    else if(addr >= 0x2008 && addr <= 0x3fff){
        uint16_t tmpaddr = (addr & 0x0007) + 0x2000;
        switch(tmpaddr){
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

    else if(addr == 0x4014){
        
    }

    else{
        
        cout << "WRITING OUT OF PPU MMIO RANGE!!!" << endl;

    }
}



//memory operations (PPU <-> MEMORY)

uint8_t PPUBus :: read_mem(uint16_t addr){
    return vram[addr];
}









