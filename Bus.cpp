#include <cstdint>
#include <array>
#include "Bus.h"

Bus::Bus(){
    //clear the ram
    for(auto &data : ram){
        data = 0x00;
    }
}




