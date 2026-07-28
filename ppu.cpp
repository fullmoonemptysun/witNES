#include "ppu.h"
#include <iostream>
#include "disasm.h"

#define CAST_15(x) ((x) & 0x7FFF)
#define CAST_14(x) ((x) & 0x3fff)

using namespace std;




uint8_t ppu::read_nt(int index){

    uint8_t base_nt_addr = (vreg & 0xc00) >> 10;
    
    if(bus->mainbus->cart->mirroring){
        //horizontal mirroring

        switch(base_nt_addr){
            case 0:
                return read(0x2000 + index);
            case 1:
                return read(0x2400 + index);
            case 2: //0x2800
                return read(0x2000 + index);
            
            case 3: //0x2c00
                return read(0x2400 + index);

        }

    }

    else{
        //vertical mirroring
        switch(base_nt_addr){
            case 0:
                return read(0x2000 + index);
            case 1: // 0x2400
                return read(0x2000 + index);
            case 2: //0x2800
                return read(0x2400 + index);
            
            case 3: //0x2c00
                return read(0x2400 + index);

        }


    }
}
uint16_t ppu::read_pt(int tileno){
    //tileno * 16 + vertical pixel offset + plane * 8 
    uint16_t pt_addr = tileno * 16 + ((vreg & 0x7000) >> 12) + 0 * 8; //address for left pt lobyte
    uint8_t pt_base_addr = ppuctrl & 0b00010000; // 0 means $0000 non 0 means $1000

    if(pt_base_addr){
        pt_addr |= 0x1000;
        uint8_t lobyte = bus->read_mem(pt_addr);
        uint8_t hibyte = bus->read_mem(pt_addr + 8);

        return ((hibyte << 8) + lobyte);
    }

    else{
        uint8_t lobyte = bus->read_mem(pt_addr);
        uint8_t hibyte = bus->read_mem(pt_addr + 8);

        return ((hibyte << 8) + lobyte);
        
    }

    

}
void ppu::clock(){

    //idle cycle
    if((scanline >= 0 && scanline <= 239) && dot == 0){
        dot += 1;
    }

    //VISUAL DOTS AND SCANLINES
    else if((scanline >= 0 && scanline <= 239) && (1 <= dot && dot <= 256) && (sp_render_enable || bg_render_enable)){
        
        //render current dot first
        
        

        //if 8 dot boundary, then do mem fetches
        if((dot == 1) || (dot % 9 == 0)){
            //get tile no. from nametable
            int row = scanline/8;
            int col = dot/8;

            uint8_t tileno = read_nt((32*row + col));

            uint16_t ptdata = read_pt(tileno);

            //Set the shift registers next 8 bits 
            shft_reg_hi |= (ptdata  >> 8);
            shft_reg_lo |= (ptdata & 0x00ff);

            
            
            
            //debugging 
            cout << "0x" << toHex(tileno) << endl;


            

            


        }

        
    }

}
