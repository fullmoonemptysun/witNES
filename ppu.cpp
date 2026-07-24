#include "ppu.h"


using namespace std;


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

            
        }

        
    }

}
