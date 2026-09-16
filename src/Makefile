



all: witnes
	@make clean
	@make witnes


run_test: witnes
	@./witnes ../nestest.nes > logs.txt

witnes: disasm.cpp cpu.cpp ppu.cpp ppubus.cpp cartridge.cpp Bus.cpp main.cpp ../display/display.cpp
	@g++ -o witnes ./disasm.cpp ./cpu.cpp ./cartridge.cpp ./Bus.cpp ./ppubus.cpp ./ppu.cpp ../display/display.cpp ./main.cpp /usr/lib/x86_64-linux-gnu/libSDL2.so -I/home/anubis/projects/NES/witNES/display/ -I/usr/include/SDL2/
	@chmod u+x ./witnes


display: ../display/display.cpp
	@g++ -o ../display/display ../display/display.cpp /usr/lib/x86_64-linux-gnu/libSDL2.so -I/usr/lib/x86_64-linux-gnu/libSDL2.so



clean:
	@rm witnes 
