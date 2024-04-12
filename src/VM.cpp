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

void VM::run() {
    std::cout << "Executing " << mInstCount << " instructions... " << std::endl;
    while(mPC < mInstCount) {
        executeInstruction();
    }

    // Print GPRs
    std::cout << "Final register values: " << std::endl;
    for(int i = 0; i < GPR_COUNT; ++i) {
        std::cout << "R" << i << ": 0x" << std::hex << int(mGPRs[i]) << "\n";
    }

    std::cout << "MEMA: 0x" << std::hex << int(mGPRs[GPR_COUNT]) << "\n";
    std::cout << "INSTA: 0x" << std::hex << int(mGPRs[GPR_COUNT + 1]) << "\n";
    std::cout << "IR: 0x" << std::hex << int(mIR) << "\n";
    std::cout << "PC: 0x" << std::hex << int(mPC) << "\n";
    std::cout << "ACC: 0x" << std::hex << int(mACC) << "\n";
    std::cout << std::flush;
}

void VM::executeInstruction() {
    // Fetch
    mIR = mROM[mPC];
    ++mPC;

    OP_CODE op = static_cast<OP_CODE>((mIR & 0xF0) >> 4);
    uint8_t operand = mIR & 0x0F;

    switch(op) {
        case OP_CODE::MVRA:
            mACC = getRegister(operand);
            break;
        case OP_CODE::MVAR:
            if(operand != static_cast<uint8_t>(SPECIAL_REG::ZERO) &&
                operand != static_cast<uint8_t>(SPECIAL_REG::ONE) &&
                operand != static_cast<uint8_t>(SPECIAL_REG::MIN1)) {
                    getRegister(operand) = mACC;
                } else {
                    std::cout << "Warning: attempting to modify constant register"
                        << std::endl;
                }
            break;
        case OP_CODE::LDA:
            mACC = mRAM[getRegister(static_cast<uint8_t>(SPECIAL_REG::MEMA))];
            break;
        case OP_CODE::STA:
            mRAM[getRegister(static_cast<uint8_t>(SPECIAL_REG::MEMA))] = mACC;
            break;
        case OP_CODE::MVAH:
            mACC &= 0x0F;
            mACC |= operand << 4;
            break;
        case OP_CODE::MVAL:
            mACC &= 0xF0;
            mACC |= 0x0F & operand;
            break;
        case OP_CODE::JUMPZ:
            if(mACC == 0) {
                mPC = getRegister(static_cast<uint8_t>(SPECIAL_REG::INSTA));
            }
            break;
        case OP_CODE::JUMPN:
            if(mACC & 0x80) {
                std::cout << "JUMP" << std::endl;
                mPC = getRegister(static_cast<uint8_t>(SPECIAL_REG::INSTA));
            } else { std::cout << "NO JUMP" << std::endl;}
            break;
        case OP_CODE::ADD:
            mACC += getRegister(operand);
            break;
        case OP_CODE::SUB:
            mACC -= getRegister(operand);
            break;
        case OP_CODE::MUL:
            mACC *= getRegister(operand);
            break;
        case OP_CODE::LLS:
            mACC <<= (getRegister(operand) & 0b111);
            break;
        case OP_CODE::LRS:
            mACC >>= (getRegister(operand) & 0b111);
            break;
        case OP_CODE::AND:
            mACC &= getRegister(operand);
            break;
        case OP_CODE::OR:
            mACC |= getRegister(operand);
            break;
        case OP_CODE::XOR:
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

    SPECIAL_REG reg = static_cast<SPECIAL_REG>(id);
    switch(reg) {
        case SPECIAL_REG::ZERO:
            return mZERO;
        case SPECIAL_REG::ONE:
            return mONE;
        case SPECIAL_REG::MIN1:
            return mMIN1;
        default:
            std::cerr << "Invalid register 0x" << std::hex << int(id) << std::endl;
            return mZERO;
    }
}
