#include <string>
#include <cstdint>
#include <algorithm>


using namespace std;

const char hexstr[16] = {'0', '1', '2', '3', '4','5','6','7','8','9','A','B','C','D','E','F'};

//int to hex
//addresses/16 bit
string toHex(uint16_t addr);
string toHex(uint8_t val);


