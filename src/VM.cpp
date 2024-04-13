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

            mROM[instCount] = inst;
            ++instCount;
        }
    } else {
        std::cerr << "Unable to open file: " << progPath << std::endl;
    }

    mInstCount = instCount;
}

void VM::run(bool dumpMem) {
    std::cout << "Executing " << mInstCount << " instructions... " << std::endl;
    // In reality, we would go on until instruction 255, but it will yield
    // the same result as the last instruction + 1.
    while(mPC < mInstCount + 1) {
        executeInstruction();
    }

    // Print GPRs
    std::cout << "Final register values: " << std::endl;
    std::cout << std::setfill('0');
    for(int i = 0; i < GPR_COUNT; ++i) {
        std::cout << "R" << i << ": 0x" << std::hex << std::setw(2) << int(mGPRs[i]) << "\n";
    }

    std::cout << "MEMA: 0x" << std::hex << std::setw(2) << int(mGPRs[GPR_COUNT]) << "\n";
    std::cout << "INSTA: 0x" << std::setw(2) << int(mGPRs[GPR_COUNT + 1]) << "\n";
    std::cout << "IR: 0x" << std::setw(2) << int(mIR) << "\n";
    std::cout << "PC: 0x" << std::setw(2) << int(mPC) << "\n";
    std::cout << "ACC: 0x" << std::setw(2)  << int(mACC) << "\n";

    if(dumpMem) {
        std::cout << "\nMemory dump:\n";
        for(int i = 0; i < RAM_SIZE; ++i) {
            std::cout << std::hex << std::setw(2) << int(mRAM[i]) << ' ';
            if((i + 1) % 16 == 0) std::cout << '\n';
        }
        std::cout << '\n';
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
            if(operand != static_cast<uint8_t>(SpecialReg::ZERO) &&
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
    if(id < GPR_COUNT + SPECIAL_GPR_COUNT) {
        return mGPRs[id];
    }

    SpecialReg reg = static_cast<SpecialReg>(id);
    switch(reg) {
        case SpecialReg::ZERO:
            return mZERO;
        case SpecialReg::ONE:
            return mONE;
        case SpecialReg::MIN1:
            return mMIN1;
        default:
            std::cerr << "Invalid register 0x" << std::hex << int(id) << std::endl;
            return mZERO;
    }
}
