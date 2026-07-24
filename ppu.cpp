#include "ppu.h"


using namespace std;




uint8_t ppu::read_nt(int index){

    uint8_t base_nt_addr = ppuctrl & 0b00000011;
    
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

void ppu::clock(){

    //idle cycle
    if((scanline >= 0 && scanline <= 239) && dot == 0){
        dot += 1;
    }

    //VISUAL DOTS AND SCANLINES
    else if((scanline >= 0 && scanline <= 239) && (1 <= dot && dot <= 256) && (sp_render_enable || bg_render_enable)){
        //8 dot boundary
        if((dot == 1) || (dot % 9 == 0)){
            //get tile no. from nametable
            int row = scanline/8;
            int col = dot/8;

            uint8_t tileno = read_nt((32*row + col));
            

            


        }

        
    }

}
