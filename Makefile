witnes: disasm.cpp cpu.cpp cartridge.cpp Bus.cpp main.cpp
	g++ -W -o witnes ./disasm.cpp ./cpu.cpp ./cartridge.cpp ./Bus.cpp ./main.cpp



clean:
	rm witnes 
