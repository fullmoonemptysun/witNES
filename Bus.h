#ifndef BUS_H
#define BUS_H
#include "cpu.h"
#include "cartridge.h"
#include <array>
#include <cstdlib>
#include <cstdint>

using namespace std;

class Bus
{
private:
    /* data */
public:
    Bus(const string& filename);
    ~Bus() {};

public://devices on bus
    cpu witcpu;
    Cartridge* cart;
   

    array<uint8_t, 64 * 128> ram; //Fake RAM


public:

    //no need for read or write signals because these methods state what operation is being done.
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr, bool bReadOnly = false);
};


#endif
