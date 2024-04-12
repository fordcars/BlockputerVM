#pragma once
#include <string>

#define ROM_SIZE 256
#define RAM_SIZE 256

class VM {
public:
    VM(const std::string& progPath);
    void run();
private:
    uint8_t mROM[ROM_SIZE] {};
    uint8_t mRAM[RAM_SIZE] {};
    int mInstCount = 0;
};
