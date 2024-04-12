#include "VM.hpp"
#include <fstream>
#include <iostream>

VM::VM(const std::string& progPath) {
    std::ifstream file(progPath);
    int instCount = 0;

    // Load instructions to ROM
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line) && instCount < 256) {
            uint8_t inst = 0;
            int bit = 8;
            for(char c : line) {
                if(bit > 0 && c == '1' || c == '0') {
                    --bit;
                    if(c == '1') {
                        inst |= uint8_t{1} << bit;
                    }
                }
            }

            mROM[instCount] = inst;
            ++instCount;
        }
    } else {
        std::cerr << "Unable to open file: " << progPath << std::endl;
    }

    mInstCount = instCount;
}

void VM::run() {
    for(int i = 0; i < mInstCount; ++i) {
        std::cout << std::hex << int(mROM[i]) << std::endl;
    }
}