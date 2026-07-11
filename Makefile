
all: witnes
	make clean
	make witnes


run: witnes
	./witnes ../nestest.nes

witnes: disasm.cpp cpu.cpp cartridge.cpp Bus.cpp main.cpp
	g++ -o witnes ./disasm.cpp ./cpu.cpp ./cartridge.cpp ./Bus.cpp ./main.cpp
	chmod u+x ./witnes



clean:
	rm witnes 
