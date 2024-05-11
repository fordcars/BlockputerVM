#pragma once
#include <string>

#define ROM_SIZE 256
#define RAM_SIZE 256
#define GPR_COUNT 9
#define SPECIAL_REG_COUNT 7

enum class OpCode : uint8_t {
    MVRA  = 0b0000,
    MVAR  = 0b0001,
    STA   = 0b0010,
    LDA   = 0b0011,
    JUMPN = 0b0100,
    JUMPZ = 0b0101,
    MVAH  = 0b0110,
    MVAL  = 0b0111,
    MUL   = 0b1000,
    SUB   = 0b1001,
    ADD   = 0b1010,
    LLS   = 0b1011,
    LRS   = 0b1100,
    AND   = 0b1101,
    OR    = 0b1110,
    XOR   = 0b1111
};

enum class SpecialReg : uint8_t {
    OUT   = 0b1001,
    IN    = 0b1010,
    MEMA  = 0b1011,
    INSTA = 0b1100,
    ZERO  = 0b1101,
    ONE   = 0b1110,
    MIN1  = 0b1111
};

class VM {
public:
    VM(const std::string& progPath);
    void run(bool printDecimal, bool dumpMem);

private:
    int mInstCount = 0;
    uint8_t mROM[ROM_SIZE] {};
    uint8_t mRAM[RAM_SIZE] {};

    // Registers
    uint8_t mREGs[GPR_COUNT + SPECIAL_REG_COUNT] {};
    uint8_t mIR   = 0;
    uint8_t mPC   = 0;
    uint8_t mACC  = 0;

    void executeInstruction();
    uint8_t& getRegister(uint8_t id);
};
