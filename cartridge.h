
#ifndef CART_H
#define CART_H

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include "disasm.h"

using namespace std;




/**
 * The actual cartridge where all the data will be loaded from the ines file.
 * 
 * TODO: I am using only 1 kind of mapper (NROM-128/256) for now. So I do not read the mapper nibbles. Fix this when extending for more games.
 */

class Bus;
class Cartridge {

    public:
         Cartridge(const string &filename){


            // cout << "CREATING CARTRIDGE NOW" << '\n';
            header.resize(16);
            ifstream file(filename, ios::binary);
            //file not opened 
            if(!file.is_open()){
                cout << "please check if file exists!!" << endl; 
                return;
            }

            //read header
            file.read((char *)header.data(),16); //cast because .read expects a char* buffer (C-style function).



            //DEBUG iNES HEADER -------------------------------------------
            // cout << "[ " ;
            // for (int i = 0; i < header.size(); i++)
            // {
                //cout << ((int)(header.at(i))) << ", ";
            // }
            // cout << " ]" << endl;
            //DEBUg iNES HEADER ---------------------------------------------


            
//debug
            // cout << "HEADER CREATED, CREATING PRG_ROM" << '\n';
            
            prg_rom.resize( 16384 * header[4]);
          

            //trainer exists
            if(header[6] & 0b00000100){
                cout << "Trainer exists!" << endl;
                trainer.resize(512);
                //read trainer
                file.read((char*)trainer.data(), 512);
            }

            file.read((char*)prg_rom.data(), prg_rom.size());

            // cout << "PRG_ROM CREATED!" << '\n';

            //CHR-ROM exists
            if(header[5] != 0){
                chr_rom.resize( 8192 * header[5]);
                file.read((char*)chr_rom.data(), chr_rom.size());
            }


            this->mapper = (((header[6] & 0b11110000)>>4) | (header[7] & 0b11110000));
            // cout << "MAPPER READ: " << (int)(this->mapper) << '\n';



         };
       

        void ConnectBus(Bus* n){bus = n;}

        uint8_t cpu_read(uint16_t addr);
        uint8_t cpu_write(uint16_t addr, uint8_t value);

        //TODO: Implement PPU's read and write
        uint8_t mapper = 0x00;



    private:
        

        vector<uint8_t> header;
        vector<uint8_t> trainer;
        vector<uint8_t> prg_rom;
        vector<uint8_t> chr_rom;
        Bus * bus;
        uint8_t data= 0x00;


};


#endif
