#include "VM.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

#define COMMENT_CHAR ';'

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
                if(c == COMMENT_CHAR) break;
                if(bit > 0 && (c == '1' || c == '0')) {
                    --bit;
                    if(c == '1') {
                        inst |= uint8_t{1} << bit;
                    }
                }
            }

            if(bit == 0) {
                mROM[instCount] = inst;
                ++instCount;
            }
        }
    } else {
        std::cerr << "Unable to open file: " << progPath << std::endl;
    }

    mInstCount = instCount;

    // Setup special registers
    mREGs[static_cast<uint8_t>(SpecialReg::ZERO)] = 0;
    mREGs[static_cast<uint8_t>(SpecialReg::ONE)]  = 1;
    mREGs[static_cast<uint8_t>(SpecialReg::MIN1)] = 255;
}

void VM::run(bool printDecimal, bool dumpMem) {
    std::cout << "Executing " << mInstCount << " instructions... " << std::endl;
    // In reality, we would go on until instruction 255, but it will yield
    // the same result as the last instruction + 1.
    while(mPC < mInstCount + 1) {
        executeInstruction();
    }

    // Print GPRs
    auto* numFormat = printDecimal ? std::dec : std::hex;
    int valueWidth  = printDecimal ? 3 : 2;

    std::cout << "Final register values: " << std::endl;
    std::cout << std::setfill('0');

    for(int i = 0; i < GPR_COUNT; ++i) {
        std::cout << "R" << i << ": " << *numFormat << std::setw(valueWidth) << int(mREGs[i]) << "\n";
    }

    std::cout << "OUT: " << *numFormat << std::setw(valueWidth) << int(mREGs[static_cast<uint8_t>(SpecialReg::OUT)]) << "\n";
    std::cout << "MEMA: " << *numFormat << std::setw(valueWidth) << int(mREGs[static_cast<uint8_t>(SpecialReg::MEMA)]) << "\n";
    std::cout << "INSTA: " << std::setw(valueWidth) << int(mREGs[static_cast<uint8_t>(SpecialReg::INSTA)]) << "\n";
    std::cout << "IR: " << std::setw(valueWidth) << int(mIR) << "\n";
    std::cout << "PC: " << std::setw(valueWidth) << int(mPC) << "\n";
    std::cout << "ACC: " << std::setw(valueWidth)  << int(mACC) << "\n";

    if(dumpMem) {
        std::cout << "\nMemory dump:\n";
        for(int i = 0; i < RAM_SIZE; ++i) {
            std::cout << *numFormat << std::setw(valueWidth) << int(mRAM[i]) << ' ';
            if((i + 1) % 16 == 0) std::cout << '\n';
        }
    }
    std::cout << std::flush;
}

void VM::executeInstruction() {
    // Fetch
    mIR = mROM[mPC];
    ++mPC;

    OpCode op = static_cast<OpCode>((mIR & 0xF0) >> 4);
    uint8_t operand = mIR & 0x0F;

    switch(op) {
        case OpCode::MVRA:
            mACC = getRegister(operand);
            break;
        case OpCode::MVAR:
            if(operand != static_cast<uint8_t>(SpecialReg::IN) &&
                operand != static_cast<uint8_t>(SpecialReg::ZERO) &&
                operand != static_cast<uint8_t>(SpecialReg::ONE) &&
                operand != static_cast<uint8_t>(SpecialReg::MIN1)) {
                    getRegister(operand) = mACC;
                } else {
                    std::cout << "Warning: attempting to modify constant register"
                        << std::endl;
                }
            break;
        case OpCode::LDA:
            mACC = mRAM[getRegister(static_cast<uint8_t>(SpecialReg::MEMA))];
            break;
        case OpCode::STA:
            mRAM[getRegister(static_cast<uint8_t>(SpecialReg::MEMA))] = mACC;
            break;
        case OpCode::MVAH:
            mACC &= 0x0F;
            mACC |= operand << 4;
            break;
        case OpCode::MVAL:
            mACC &= 0xF0;
            mACC |= 0x0F & operand;
            break;
        case OpCode::JUMPZ:
            if(mACC == 0) {
                mPC = getRegister(static_cast<uint8_t>(SpecialReg::INSTA));
            }
            break;
        case OpCode::JUMPN:
            if(mACC & 0x80) {
                mPC = getRegister(static_cast<uint8_t>(SpecialReg::INSTA));
            }
            break;
        case OpCode::ADD:
            mACC += getRegister(operand);
            break;
        case OpCode::SUB:
            mACC -= getRegister(operand);
            break;
        case OpCode::MUL:
            mACC *= getRegister(operand);
            break;
        case OpCode::LLS:
            mACC <<= (getRegister(operand) & 0b111);
            break;
        case OpCode::LRS:
            mACC >>= (getRegister(operand) & 0b111);
            break;
        case OpCode::AND:
            mACC &= getRegister(operand);
            break;
        case OpCode::OR:
            mACC |= getRegister(operand);
            break;
        case OpCode::XOR:
            mACC ^= getRegister(operand);
            break;
        default:
            std::cerr << "Invalid op code!" << std::endl;
            break;
    }
}

uint8_t& VM::getRegister(uint8_t id) {
    if(id == static_cast<uint8_t>(SpecialReg::IN)) {
        // IN register, ask for user input
        unsigned int value;
        std::cout << "User input (dec): " << std::endl;
        std::cin >> value;
        return mREGs[static_cast<uint8_t>(SpecialReg::IN)] = static_cast<uint8_t>(value);
    } else if(id < GPR_COUNT + SPECIAL_REG_COUNT) {
        return mREGs[id];
    }

    std::cerr << "Invalid register 0x" << std::hex << static_cast<int>(id)
        << std::endl;
    return mREGs[0];
}
