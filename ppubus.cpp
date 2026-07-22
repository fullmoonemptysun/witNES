#include "ppubus.h"
#include <iostream>
using namespace std;

uint8_t PPUBus::read_register(uint16_t addr){
    if(addr >= 0x2000 && addr <= 2007){
        switch(addr){
            case 0x2000:
                return witppu->ppuctrl;
            case 0x2001:
                return witppu->ppumask;
            case 0x2002:
                return witppu->ppustatus;
            case 0x2003:
                return witppu->oamaddr;
            case 0x2004:
                return witppu->oamdata;
            case 0x2005:
                return witppu->ppuscroll;      
            case 0x2006:
                return witppu->ppuaddr;    
            case 0x2007:
                return witppu->ppudata;
            
        }
    }

    else if(addr >= 0x2008 && addr <= 0x3fff){
        uint16_t tmpaddr = (addr & 0x0007) + 0x2000;
        switch(tmpaddr){
            case 0x2000:
                return witppu->ppuctrl;
            case 0x2001:
                return witppu->ppumask;
            case 0x2002:
                return witppu->ppustatus;
            case 0x2003:
                return witppu->oamaddr;
            case 0x2004:
                return witppu->oamdata;
            case 0x2005:
                return witppu->ppuscroll;      
            case 0x2006:
                return witppu->ppuaddr;    
            case 0x2007:
                return witppu->ppudata;
            
        }

    }


    else{
        
        cout << "READIN OUT OF PPU MMIO RANGE!!!" << endl;

        return 0;
    }
}


void PPUBus::write_register(uint16_t addr, uint8_t data){
    if(addr >= 0x2000 && addr <= 2007){
        switch(addr){
            case 0x2000:
                 witppu->ppuctrl = data;
            case 0x2001:
                 witppu->ppumask = data;
            case 0x2002:
                 witppu->ppustatus = data;
            case 0x2003:
                 witppu->oamaddr = data;
            case 0x2004:
                 witppu->oamdata = data;
            case 0x2005:
                 witppu->ppuscroll = data;      
            case 0x2006:
                 witppu->ppuaddr = data;    
            case 0x2007:
                 witppu->ppudata = data;
            
        }
    }

    else if(addr >= 0x2008 && addr <= 0x3fff){
        uint16_t tmpaddr = (addr & 0x0007) + 0x2000;
        switch(tmpaddr){
            case 0x2000:
                 witppu->ppuctrl = data;
            case 0x2001:
                 witppu->ppumask = data;
            case 0x2002:
                 witppu->ppustatus = data;
            case 0x2003:
                 witppu->oamaddr = data;
            case 0x2004:
                 witppu->oamdata = data;
            case 0x2005:
                 witppu->ppuscroll = data;      
            case 0x2006:
                 witppu->ppuaddr = data;    
            case 0x2007:
                 witppu->ppudata = data;
            
        }

    }

    else{
        
        cout << "WRITING OUT OF PPU MMIO RANGE!!!" << endl;

    }
}




