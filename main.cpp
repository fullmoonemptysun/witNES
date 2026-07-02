#include <iostream>
#include "cpu.h"
#include "Bus.h"
using namespace std;

int main(int argc, char const *argv[])
{




        Bus bus(argv[1]);
        bus.cpu.clock(); // start the CPU

        bus.cpu.clock(); // 1st cycle
        bus.cpu.clock(); // 2nd cycle
        
        

}
