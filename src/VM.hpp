#pragma once
#include <string>

#define ROM_SIZE 256
#define RAM_SIZE 256
#define GPR_COUNT 6
#define SPECIAL_GPR_COUNT 2  // MEMA + INSTA

enum class OpCode : uint8_t {
    MVRA  = 0b0000,
    MVAR  = 0b0001,
    LDA   = 0b0011,
    STA   = 0b0010,
    MVAH  = 0b0110,
    MVAL  = 0b0111,
    JUMPZ = 0b0101,
    JUMPN = 0b0100,
    ADD   = 0b1100,
    SUB   = 0b1101,
    MUL   = 0b1111,
    LLS   = 0b1110,
    LRS   = 0b1010,
    AND   = 0b1011,
    OR    = 0b1001,
    XOR   = 0b1000
};

enum class SpecialReg : uint8_t {
    MEMA  = 0b0110,
    INSTA = 0b0111,
    ZERO  = 0b1000,
    ONE   = 0b1001,
    MIN1  = 0b1010,
    IR    = 0b1011,
    PC    = 0b1100
};

class VM {
public:
    VM(const std::string& progPath);
    void run();

private:
    int mInstCount = 0;
    uint8_t mROM[ROM_SIZE] {};
    uint8_t mRAM[RAM_SIZE] {};

    // Registers
    uint8_t mGPRs[GPR_COUNT + SPECIAL_GPR_COUNT] {};
    uint8_t mZERO = 0; // Should be constant
    uint8_t mONE  = 1; // Should be constant
    uint8_t mMIN1 = 255;
    uint8_t mIR   = 0;
    uint8_t mPC   = 0;
    uint8_t mACC  = 0;

    void executeInstruction();
    uint8_t& getRegister(uint8_t id);
};
