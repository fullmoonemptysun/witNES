cmake_minimum_required(VERSION 3.16)
project(MyProgram)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MyProgram
    main.cpp
    Bus.cpp
    cartridge.cpp 
    disasm.cpp 
    cpu.cpp 

)