#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;

/**
 * The actual cartridge where all the data will be loaded from the ines file.
 */

class Cartridge {

    public:
         Cartridge(const string &filename){
            header.resize(16);
            ifstream file(filename, ios::binary);
            //file not opened 
            if(!file.is_open()){
                cout << "please check if file exists!!" << endl; 
                return;
            }

            //read header
            file.read((char *)header.data(),16);

            trainer.resize(512);
            prg_rom.resize( 16384 * header[4]);
            chr_rom.resize(8192 * header[5]);
         };
        ~Cartridge();

        uint8_t read(uint16_t addr);
        uint8_t write(uint16_t addr, uint8_t value);



    private:

        vector<uint8_t> header;
        vector<uint8_t> trainer;
        vector<uint8_t> prg_rom;
        vector<uint8_t> chr_rom;


};
