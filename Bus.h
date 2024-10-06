#include "wit6502.h"
#include <array>
#include <cstdlib>
#include <cstdint>

using namespace std;

class Bus
{
private:
    /* data */
public:
    Bus(/* args */);
    ~Bus();

public://devices on bus
    wit6502 cpu;
    array<uint8_t, 64 * 128> ram; //Fake RAM


public:

    //no need for read or write signals because these methods state what operation is being done.
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr, bool bReadOnly = false);
};