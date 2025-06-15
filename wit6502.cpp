#include <cstdlib>
#include <cstdint>
#include "Bus.h"
#include <iostream>


using namespace std; 

//----------------IMPLEMENT ALL CLASS FUNCTIONS-------------------
void wit6502::ConnectBus(Bus* n){bus = n;}

//Write to the bus
void wit6502::write(uint16_t a, uint8_t d){
        bus -> write(a, d); //Call bus's write method
};

//read from the bus
uint8_t wit6502::read(uint16_t addr){
//This is the same as (bus*).read(...)
        return bus -> read(addr, false);
}





