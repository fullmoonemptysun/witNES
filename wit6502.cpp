#include <cstdlib>
#include <cstdint>
#include "Bus.h"


using namespace std;


class wit6502
{
private:
    Bus *bus = nullptr; //initial bus state
public:
    wit6502(/* args */);
    ~wit6502();    


public:

    void ConnectBus(Bus* n){bus = n;}

    //Write to the bus
    void wit6502::write(uint16_t a, uint8_t d){
        bus -> write(a, d); //Call bus's write method
    };

    //read from the bus
    uint8_t wit6502::read(uint16_t addr){
        //This is the same as (bus*).read(...)
        return bus -> read(addr, false);
    }



};



