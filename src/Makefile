



all: witnes
	@make clean
	@make witnes


run_test: witnes
	@./witnes ../nestest.nes > logs.txt

witnes: disasm.cpp cpu.cpp ppu.cpp ppubus.cpp cartridge.cpp Bus.cpp main.cpp
	@g++ -o witnes ./disasm.cpp ./cpu.cpp ./cartridge.cpp ./Bus.cpp ./ppubus.cpp ./ppu.cpp ./main.cpp
	@chmod u+x ./witnes



clean:
	@rm witnes 
