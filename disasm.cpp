#include <algorithm>
#include "disasm.h"
#include <iostream>


using namespace std;

string toHex(uint16_t dec){
    string hex = "";
    if(dec == 0x0000){
        return "0000";
    }
    while (dec != 0){
        hex += hexstr[dec % 16];
        dec /= 16;
        
    }

    while(hex.size() < 4){
        hex += '0';
    }

    reverse(hex.begin(), hex.end());

    return hex;
}


string toHex(uint8_t val){
    string hex = "";
    if(val == 0x00){
        return "00";
    }
    while (dec != 0){
        hex += hexstr[val % 16];
        val /= 16;
        
    }

    while(hex.size() < 2){
        hex += '0';
    }

    reverse(hex.begin(), hex.end());

    return hex;
}



int main(int argc, char const *argv[])
{
    
    int val = atoi(argv[1]);

    cout << toHex((uint16_t)val)<< endl;
    return 0;
}


