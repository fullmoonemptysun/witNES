#include <cstdlib>
#include <cstdint>
#include <iostream>
#include "Bus.h"
#include "disasm.h"



using namespace std; 


//GLOBAL DISASSEMBLY STATEMENT
struct disst {
    string addr;
    vector<string> bytes;
    string opname;
   
};


disst dislog;

//----------------IMPLEMENT ALL CLASS FUNCTIONS-------------------
void cpu::ConnectBus(Bus* n){bus = n;}

//Write to the bus
void cpu::write(uint16_t a, uint8_t d){
        bus -> write(a, d); //Call bus's write method
};

//read from the bus
uint8_t cpu::read(uint16_t addr){
//This is the same as (bus*).read(...)
        return bus -> read(addr, false);
}

//reset signal (if needed)
void cpu::reset(){
    acc = 0x00;
    xreg = 0x00;
    yreg = 0x00;
    stkp = 0xfd; //stack pointer
    // pc = read(0xFFFC)|(read(0xFFFD) << 8); correct way
    pc = 0xC000; // to test nestest HARDCODED
    status = 0b00100100;
}

void cpu::clock(){
    if(cycle == 0){


        // //debug
        // cout << "LAST CYCLE ENDED!" << '\n';
        // cout << "READING OPCODE..." << '\n';
        opcode = read(pc);

        //DEBUG
        
        // cout << "OPCODE READ: "<< toHex(opcode) << '\n';

        //Disassembly---------------------------------------------------------
        dislog.addr = toHex(pc);
        dislog.bytes.push_back(toHex((uint8_t)opcode));

        //2 byte instructions
        if((this->lookup[opcode].addrmode == &cpu::REL)||(this->lookup[opcode].addrmode == &cpu::IMM)||(this->lookup[opcode].addrmode == &cpu::ZP0)||(this->lookup[opcode].addrmode == &cpu::ZPX) || (this->lookup[opcode].addrmode == &cpu::ZPY) || (this->lookup[opcode].addrmode == &cpu::IZX) || (this->lookup[opcode].addrmode == &cpu::IZY)){
            dislog.bytes.push_back(" ");
            dislog.bytes.push_back(toHex((uint8_t)(read(pc+1))));
            dislog.bytes.push_back("    ");
            dislog.opname = this->lookup[opcode].name;
        }

        //3 byte instructions
        else if((this->lookup[opcode].addrmode == &cpu::ABS)||(this->lookup[opcode].addrmode == &cpu::ABX) || (this->lookup[opcode].addrmode == &cpu::ABY) || (this->lookup[opcode].addrmode == &cpu::IND)){
            dislog.bytes.push_back(" ");
            dislog.bytes.push_back(toHex((uint8_t)(read(pc+1))));
            dislog.bytes.push_back(" ");
            dislog.bytes.push_back(toHex((uint8_t)(read(pc+2))));
            dislog.bytes.push_back(" ");
            dislog.opname = this->lookup[opcode].name;
        }

        //1 byte instruction
        else if(this->lookup[opcode].addrmode == &cpu::IMP){
            dislog.bytes.push_back("       ");
            dislog.opname = this->lookup[opcode].name;
        }

        
        //disassembly------

        cout << dislog.addr << "  ";
        for(string &str:dislog.bytes){
            cout << str;
        }
        cout << ' ' << dislog.opname << "  " << "A:" << toHex(acc) << ' ' << "X:" << toHex(xreg) << ' '<< "Y:" << toHex(yreg) << ' ' << "P:" << toHex(status)<< ' ' << "SP:" << toHex(stkp) << endl;

        dislog.bytes.clear();

        //-----------------

     


        pc++;



        

        cycle = lookup[opcode].cycles;
        (this->*lookup[opcode].addrmode)();
        (this->*lookup[opcode].operate)();
        

        cycle -= 1;
    }


	else{
		cycle -= 1;  //cycles reduces with every tick
	}
}

using a = cpu;

cpu::cpu(){
    lookup = 
	{
		{ "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
		{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	};
}


//----------FLAGS (STATUS REGISTER) SETTER and GETTER
void cpu::setFlag(FLAGSTAT flag, bool b){
	//set the flag
	if(b){
		status |= flag; //set the flag
	}

	//clear the flag
	else{
        
		status &= ~flag;

	}
}

uint8_t cpu::getFlag(FLAGSTAT flag){
	//return 1 if set, 0 if not set
	return (status & flag) ? 1:0;
}
//---------------------------------------------------




/**
 * ---------------addrmode functions--------------------
 * It is important that these functions only operate on calculation of effective addresses
 * and nothing else. They should only set the addr_main variable. They should never read/write to an address
 * unless that address contains bytes for the effective address. 
 */

//IMM
uint8_t cpu::IMM(){
	//the next byte after opcode is the 8 bit operand.
	addr_main = pc; 
    pc++;
	
	return pc;
}

//REL (8 bit signed offset from current PC)
uint8_t cpu::REL(){
    //the next byte after opcode is the 8 bit signed offset

    int offset = read(pc);
    pc++;
    addr_main = pc + offset;

    if(((pc ^ addr_main) & 0xff00)){

        page_crossed = 1;
    }

    return offset;
}

//ZP0
uint8_t cpu::ZP0(){
	//Read from the 0 page
	uint8_t addr = read(pc);
	pc++;
	addr_main = addr;
    return 0;
}

//ZPX ($00, X)
uint8_t cpu::ZPX(){
	//Effective address calculation
	uint8_t addr = ((read(pc) + xreg) % 256);
	pc++;

	// addr_main = 7 >> addr; no need to shift upper bytes will be 0 anyway. 
    addr_main = addr;
    return 0;
}

//ZPY ($00, Y)
uint8_t cpu::ZPY(){
	//Effective address calculation
	uint8_t addr = ((read(pc) + yreg) % 256);
	pc++;
	
    addr_main = addr;
    return 0;
}

//ABS $0000
uint8_t cpu::ABS(){
	//Get the low byte of the address
	uint8_t lowByte = read(pc);
	pc++;
	//Get the hight byte
	uint8_t highByte = read(pc);
	pc++;

	//effective address
	uint16_t addr = (lowByte + (highByte * 256));

	addr_main = addr;
    return 0;
}

//ABX ($0000, X)
uint8_t cpu::ABX(){
	//low byte
	uint8_t lowByte = read(pc);
	pc++;
	//high byte
	uint8_t highByte = read(pc);
	pc++;


	//effective address
	uint16_t addr = (((lowByte + (highByte * 256)) + xreg) & 0xFFFF);




    //Check for page crossing 
    if(!((highByte * 256) == (addr & 0xFF00) )){


        //all opcodes that will be effected by a page cross (cycle + 1)
        switch(opcode){
            case 0x7d:
            case 0x3d:
            case 0xdd:
            case 0x5d:
            case 0xbd:
            case 0xbc:
            case 0x1d:
            case 0xfd:
                page_crossed = 1;
                break;

        }
    }

	addr_main = addr; // ONLY NEED TO RETURN THE ADDRESS NOT READ THE DATA.
	return 0;
}


//ABY ($0000, Y)
uint8_t cpu::ABY(){
	//low byte
	uint8_t lowByte = read(pc);
	pc++;
	//high byte
	uint8_t highByte = read(pc);
	pc++;

	//effective address
	uint16_t addr = (((lowByte + (highByte * 256)) + yreg) & 0xFFFF);
        //Check for page crossing 
    if(!((highByte * 256) == (addr & 0xFF00) )){


        //all opcodes that will be effected by a page cross (cycle + 1)
        switch(opcode){
            case 0x79:
            case 0x39:
            case 0xd9:
            case 0x59:
            case 0xb9:
            case 0xbe:
            case 0x19:
            case 0xf9:
                page_crossed = 1;
                break;

        }
    }

	addr_main = addr;
	return 0;
}

//

//INDEXED INDIRECT ($00, X)
//Peek(Peek((arg + X)%256) + Peek((arg+X + 1) % 256) * 256)
uint8_t cpu::IZX(){
	uint8_t arg = read(pc);
	pc++;

	uint8_t lowByte = read((arg + xreg) % 256);
	uint16_t highByte = read(((arg + xreg + 1) % 256)) * 256;

	//Effective address
	uint16_t addr = lowByte + highByte;

	addr_main = addr;

	return 0;


}

//INDIRECT INDEXED ($00), Y
//PEEK(PEEK(arg) + PEEK((arg+1) % 256) * 256 + Y)
uint8_t cpu::IZY(){
	uint8_t arg = read(pc);
	pc++;
	uint8_t lowByte = read(arg);
	uint16_t highByte = read((arg + 1) % 256) * 256;

	//effective address
	uint16_t addr = lowByte + highByte + yreg;

    //check for page crossing
    if(!(highByte * 256 == (addr & 0xFF00))){
          //all opcodes that will be effected by a page cross (cycle + 1)
        switch(opcode){
            case 0x71:
            case 0x31:
            case 0xd1:
            case 0x51:
            case 0xb1:
            case 0x11:
            case 0xf1:
                page_crossed = 1;
                break;

        }
        
    }

	addr_main = addr;
	return 0;
}


uint8_t cpu::IMP(){

    switch(opcode){
        
        case 0x4a:
        case 0x2a:
        case 0x6a:
        case 0x0a:
            use_acc = 1;
            break;

    }


    return 0;
}

uint8_t cpu::IND(){
    uint8_t lowbyte = read(pc);
    pc++;
    uint16_t hibyte = read(pc);
    pc++;

    uint16_t addr = (hibyte << 8) + lowbyte;

    lowbyte = read(addr);
    hibyte = read(((addr+1) & 0x00FF) | (addr & 0xFF00)); //6502's JMP INDIRECT BUG. (PAGE CROSSING CANNOT HAPPEN)

    addr_main = (hibyte << 8) + lowbyte;

    return 0;
}



//------------------------Basic Instructions-----------------------------------

//Add with carry
uint8_t cpu::ADC(){

    fetched = read(addr_main);
	int result = acc + fetched + getFlag(C);



	setFlag(C, (result > 0xff));
	setFlag(Z, ((result & 0xff) == 0));
	//If the result's sign is different from both A's and memory's, signed overflow (or underflow) occurred.
	setFlag(V, (bool)((result ^ acc) & (result ^ fetched ) & 0x80));
	setFlag(N, (bool)(result & 0x80));
	acc = result & 0xff; // accumulator can only have the lower 8 bits

    if(page_crossed){
        cycle += 1;
        page_crossed = 0;
    }


	return acc;

}

//Bitwise AND
uint8_t cpu::AND(){
    fetched = read(addr_main);
    acc = acc & fetched;

    setFlag(Z, (acc == 0));
    setFlag(N, (bool)(acc & 0x80));


    if(page_crossed){
        cycle += 1;
        page_crossed = 0;
    }


    return acc;


}

//Arithmetic shift left (by 1)   C <- [76543210] <- 0
uint8_t cpu::ASL(){

    //operating on accumulator?
    if(use_acc){
        setFlag(C, (bool)(acc >> 7 & 0x01));
        acc = acc << 1;
        setFlag(Z, acc == 0x00);
        setFlag(N, (bool)(acc & 0x80));

        use_acc = 0;

    }

    //operating on memory address?
    else{

        fetched = read(addr_main);
        setFlag(C, (bool)(fetched >> 7 & 0x01));
        fetched = fetched << 1;
        setFlag(Z, fetched == 0x00);
        setFlag(N, (bool)(fetched & 0x80));

        write(addr_main, fetched);
    }

    return fetched;
}


//Branch if carry clear (BCC)
uint8_t cpu::BCC(){
    if(!(getFlag(C))){
        pc = addr_main;
        cycle += 1;
        if(page_crossed){
            cycle += 1;
            page_crossed = 0;
        }
    }

    return 0;
}



// BCS: Branch if carry flag is set
uint8_t cpu::BCS(){ 
    if((getFlag(C))){
        pc = addr_main;
        cycle += 1;
        if(page_crossed){
            cycle += 1;
            page_crossed = 0;
        }
    }

    return 0;
 }

// BEQ: Branch if zero flag is set (values equal)
uint8_t cpu::BEQ(){ 
    if((getFlag(Z))){
        pc = addr_main;
        cycle += 1;
        if(page_crossed){
            cycle += 1;
            page_crossed = 0;
        }
    }

    return 0;

 }

// BIT: Test bits in memory against accumulator
uint8_t cpu::BIT(){ 
    
    fetched = read(addr_main);
    uint8_t result = acc & fetched;
    setFlag(Z, result == 0);
    setFlag(V, (bool)(fetched & 0b01000000));
    setFlag(N, (bool)(fetched & 0x80));
    return 0; }

// BMI: Branch if negative flag is set (result minus)
uint8_t cpu::BMI(){ 
    if((getFlag(N))){
        pc = addr_main;
        cycle += 1;

        if(page_crossed){
            cycle += 1;
            page_crossed = 0;
        }

    }

    return 0;
     }

// BNE: Branch if zero flag is clear (values not equal)
uint8_t cpu::BNE(){ 
    if(!(getFlag(Z))){
        pc = addr_main;
        cycle += 1;
        if(page_crossed){
            cycle += 1;
            page_crossed = 0;
        }
    }

    return 0;
 }

// BPL: Branch if negative flag is clear (result plus)
uint8_t cpu::BPL(){ 
    if(!(getFlag(N))){
        pc = addr_main;
        cycle += 1;
        if(page_crossed){
            cycle += 1;
            page_crossed = 0;
        }
    }

    return 0;
 }

// BRK: Force software interrupt (IRQ)
uint8_t cpu::BRK(){ 
    pc++; // skip the extra byte that BRK operates with $00
    irq();

    return 0;
 }

// BVC: Branch if overflow flag is clear
uint8_t cpu::BVC(){ 
    if(!(getFlag(V))){
        pc = addr_main;
        cycle += 1;
        if(page_crossed){
            cycle += 1;
            page_crossed = 0;
        }
    }

    return 0;
 }

// BVS: Branch if overflow flag is set
uint8_t cpu::BVS(){ 
    if((getFlag(V))){
        pc = addr_main;
        cycle += 1;
        if(page_crossed){
            cycle += 1;
            page_crossed = 0;
        }
    }

    return 0;
 }

// CLC: Clear carry flag
uint8_t cpu::CLC(){ 
    
    setFlag(C, 0);
    return 0; 
}

// CLD: Clear decimal flag
uint8_t cpu::CLD(){ 
    
    setFlag(D, 0);
    return 0; 
}

// CLI: Clear interrupt disable flag
uint8_t cpu::CLI(){ 

    setFlag(I, 0);
    
    return 0; }

// CLV: Clear overflow flag
uint8_t cpu::CLV(){ 
    
    setFlag(V, 0);
    return 0; }

// CMP: Compare accumulator with memory
uint8_t cpu::CMP(){ 
    fetched = read(addr_main);
    setFlag(C, (acc >= fetched));
    setFlag(Z, (acc == fetched));
    setFlag(N, (bool)((acc - fetched) & 0x80));

    return 0; }

// CPX: Compare X register with memory
uint8_t cpu::CPX(){ 
    fetched = read(addr_main);
    setFlag(C, (xreg >= fetched));
    setFlag(Z, (xreg == fetched));
    setFlag(N, (bool)((xreg - fetched) & 0x80));

    return 0; }

// CPY: Compare Y register with memory
uint8_t cpu::CPY(){ 
    
    fetched = read(addr_main);
    setFlag(C, (yreg >= fetched));
    setFlag(Z, (yreg == fetched));
    setFlag(N, (bool)((yreg - fetched) & 0x80));

    return 0; }

// DEC: Decrement memory value by 1
uint8_t cpu::DEC(){ 
    
    fetched = read(addr_main);
    fetched -= 1;
    write(addr_main, fetched);
    setFlag(Z, fetched == 0);
    setFlag(N, (bool)(fetched & 0x80));

    
    return 0; }

// DEX: Decrement X register by 1
uint8_t cpu::DEX(){ 
    
    xreg -= 1;

    setFlag(Z, xreg == 0);
    setFlag(N, (bool)(xreg & 0x80));
    
    return 0; }

// DEY: Decrement Y register by 1
uint8_t cpu::DEY(){    
    yreg -= 1;

    setFlag(Z, yreg == 0);
    setFlag(N, (bool)(yreg & 0x80));
    
    return 0; }

// EOR: Bitwise exclusive OR memory with accumulator
uint8_t cpu::EOR(){ 
    
    fetched = read(addr_main);

    acc = acc ^ fetched;

    setFlag(Z, acc == 0);
    setFlag(N, (bool)(acc & 0x80));

    if(page_crossed){
        cycle += 1;
        page_crossed = 0;
    }
    
    return 0; }

// INC: Increment memory value by 1
uint8_t cpu::INC(){ 
    fetched = read(addr_main);
    fetched += 1;
    write(addr_main, fetched);
    setFlag(Z, fetched == 0);
    setFlag(N, (bool)(fetched & 0x80));

    
    return 0; }


// INX: Increment X register by 1
uint8_t cpu::INX(){    
    xreg += 1;

    setFlag(Z, xreg == 0);
    setFlag(N, (bool)(xreg & 0x80));
    
    return 0; }

// INY: Increment Y register by 1
uint8_t cpu::INY(){ 
    yreg += 1;

    setFlag(Z, yreg == 0);
    setFlag(N, (bool)(yreg & 0x80));
    
    return 0;
}

// JMP: Jump to new program counter location
uint8_t cpu::JMP(){ 
    
    pc = addr_main;
    return 0; 
}

// JSR: Jump to subroutine, pushing return address to stack
uint8_t cpu::JSR(){ 
    
    push(((pc-1) & 0xff00) >> 8); //push high byte
    push(pc-1); //push low byte

    pc = addr_main; // set new PC
    return addr_main; 

}

// LDA: Load memory value into accumulator
uint8_t cpu::LDA(){ 
    fetched = read(addr_main);
    acc = fetched;

    setFlag(Z, fetched == 0);
    setFlag(N, (bool)(fetched & 0x80));


    if(page_crossed){
        cycle += 1;
        page_crossed = 0;
    }

    
    return 0; 
}

// LDX: Load memory value into X register
uint8_t cpu::LDX(){ 
    
    fetched = read(addr_main);
    xreg = fetched;

    setFlag(Z, fetched == 0);
    setFlag(N, (bool)(fetched & 0x80));


    if(page_crossed){
        cycle += 1;
        page_crossed = 0;
    }

    
    return 0;  

}

// LDY: Load memory value into Y register
uint8_t cpu::LDY(){ 
    
    fetched = read(addr_main);
    yreg = fetched;

    setFlag(Z, fetched == 0);
    setFlag(N, (bool)(fetched & 0x80));


    if(page_crossed){
        cycle += 1;
        page_crossed = 0;
    }

    
    return 0; 
}

// LSR: Logical shift right by 1 bit
uint8_t cpu::LSR(){ 
    //operating on accumulator?
    if(use_acc){
        setFlag(C, (bool)(acc & 0x01));
        acc = acc >> 1;
        setFlag(Z, acc == 0x00);
        setFlag(N, 0);

        use_acc = 0;

    }

    //operating on memory address?
    else{

        fetched = read(addr_main);
        setFlag(C, (bool)(fetched & 0x01));
        fetched = fetched >> 1;
        setFlag(Z, fetched == 0x00);
        setFlag(N, 0);

    

        write(addr_main, fetched);
    }

    return fetched;

 }

// NOP: No operation, waste cycles
uint8_t cpu::NOP(){ return 0; }

// ORA: Bitwise OR memory with accumulator
uint8_t cpu::ORA(){
    
    fetched = read(addr_main);
    acc = acc | fetched;

    setFlag(Z, acc == 0);
    setFlag(N, (bool)(acc & 0x80));
   

    if(page_crossed){
        cycle += 1;
        page_crossed = 0;
    }

    return acc; 
}

// PHA: Push accumulator to stack
uint8_t cpu::PHA(){

    push(acc);
    return acc;

 }

// PHP: Push processor status flags to stack
uint8_t cpu::PHP(){ 
    uint8_t flags = status | 0b00110000;
    push(flags);

    return 0;

 }

// PLA: Pull accumulator from stack
uint8_t cpu::PLA(){ 
    
    acc = pop();
    setFlag(Z, acc == 0);
    setFlag(N, (bool)(acc & 0x80));

    return acc;

}

// PLP: Pull processor status flags from stack
uint8_t cpu::PLP(){ 
    status = (pop() & ~(B|U)) | (status & (B|U)); // TODO: I should not be changed immediately fix this when IRQ is done

    return 0;
 }

// ROL: Rotate left through carry flag
uint8_t cpu::ROL(){ 

    //operating on accumulator?
    if(use_acc){
        
        int tempC = getFlag(C);
        setFlag(C, (bool)(acc >> 7 & 0x01));
        acc = (acc << 1) | tempC ;
        setFlag(Z, acc == 0x00);
        setFlag(N, (bool)(acc & 0x80));

        use_acc = 0;

    }

    //operating on memory address?
    else{

        fetched = read(addr_main);

        int tempC = getFlag(C);
        setFlag(C, (bool)(fetched >> 7 & 0x01));
        fetched = fetched << 1 | tempC;
        setFlag(Z, fetched == 0x00);
        setFlag(N, (bool)(fetched & 0x80));

        write(addr_main, fetched);
    }

    return fetched;

 }

// ROR: Rotate right through carry flag
uint8_t cpu::ROR(){ 
    
    //operating on accumulator?
    if(use_acc){
        
        int tempC = getFlag(C);
        setFlag(C, (bool)(acc & 0x01));
        acc = (acc >> 1) | tempC << 7 ;
        setFlag(Z, acc == 0x00);
        setFlag(N, (bool)(acc & 0x80));

        use_acc = 0;

    }

    //operating on memory address?
    else{

        fetched = read(addr_main);

        int tempC = getFlag(C);
        setFlag(C, (bool)(fetched & 0x01));
        fetched = fetched >> 1 | tempC << 7;
        setFlag(Z, fetched == 0x00);
        setFlag(N, (bool)(fetched & 0x80));

        write(addr_main, fetched);
    }

    return fetched;

}

// RTI: Return from interrupt, pulling flags and PC from stack
uint8_t cpu::RTI(){ 
    status = (pop() & ~(B|U)) | (status & (B|U));
    uint8_t lo = pop();
    uint8_t hi = pop();

    pc = (hi << 8) | lo;

    return 0;
 }

// RTS: Return from subroutine, pulling PC from stack
uint8_t cpu::RTS(){ 
    uint8_t lo = pop();
    uint8_t hi = pop();

    pc = ((hi << 8) | lo) + 1;

    return 0;
 }

// SBC: Subtract memory and inverted carry from accumulator
//is used after SEC (set C = 1). So we do acc + 1's complement + 1 = two's complement subtraction. 
uint8_t cpu::SBC(){ 

    fetched = read(addr_main);
    int val = acc + ~fetched + (getFlag(C));

    setFlag(C, (bool)(!(val < 0x00)));
    setFlag(Z, val == 0);
    setFlag(V, (bool)((val ^ acc)&(val ^ ~fetched) & 0x80));
    setFlag(N, (bool)(val & 0x80));

    acc = val;

    if(page_crossed){
        cycle += 1;
        page_crossed = 0;
    }

    return 0;

 }

// SEC: Set carry flag
uint8_t cpu::SEC(){ 
    
    setFlag(C, 1);
    return 0;
}

// SED: Set decimal flag
uint8_t cpu::SED(){ 
    setFlag(D, 1);
    return 0;
 }

// SEI: Set interrupt disable flag
uint8_t cpu::SEI(){ 
    setFlag(I, 1);
    return 0; //TODO: this needs to be delayed one instruction
 }

// STA: Store accumulator value into memory
uint8_t cpu::STA(){ 
    
    write(addr_main, acc);
    return 0;

 }

// STX: Store X register value into memory
uint8_t cpu::STX(){ 
    
    write (addr_main, xreg);
    return 0;
 }

// STY: Store Y register value into memory
uint8_t cpu::STY(){ 
    write(addr_main, yreg);
    return 0; }

// TAX: Transfer accumulator to X register
uint8_t cpu::TAX(){ 
    xreg = acc;

    setFlag(Z, acc == 0);
    setFlag(N, (bool)(xreg & 0x80));
    return 0;
 }

// TAY: Transfer accumulator to Y register
uint8_t cpu::TAY(){ 
    yreg = acc;

    setFlag(Z, acc == 0);
    setFlag(N, (bool)(yreg & 0x80));
    
    return 0; }

// TSX: Transfer stack pointer to X register
uint8_t cpu::TSX(){ 
    xreg = stkp;
    setFlag(Z, stkp == 0);
    setFlag(N, (bool)(xreg & 0x80));

    return 0;
 }

// TXA: Transfer X register to accumulator
uint8_t cpu::TXA(){ 
    
    acc = xreg;
    setFlag(Z, acc == 0);
    setFlag(N, (bool)(acc & 0x80));
    
    return 0; }

// TXS: Transfer X register to stack pointer
uint8_t cpu::TXS(){
    
    stkp = xreg;

    
    return 0;
 }

// TYA: Transfer Y register to accumulator
uint8_t cpu::TYA(){ 
    
    acc = yreg;
    setFlag(Z, acc == 0);
    setFlag(N, (bool)(acc & 0x80));
    
    return 0;
}



















