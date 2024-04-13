#include "getopt.h"
#include <iostream>

#include "VM.hpp"

void printHelp(const std::string& progName) {
    std::cout
        << "Usage:\n"
        << "    " << progName << " OPTIONS\n"
        << "Options:\n"
        << "    -i        input file\n"
        << "    -d        print values as decimal (instead of hex)\n"
        << "    -m        dump memory after execution"
        << std::endl;
}

int main(int argc, char** argv) {
    int c;
    std::string progPath;
    bool dumpMem = false;
    bool printDecimal = false;

    while((c = getopt(argc, argv, "hi:dm")) != -1) {
        switch(c) {
            case 'i':
            {
                progPath = optarg;
                break;
            }
            case 'd':
                printDecimal = true;
                break;
            case 'm':
                dumpMem = true;
                break;
            case '?':
            case 'h':
            default:
                printHelp(argv[0]);
                return 0;

            case -1:
                break;
        }
    }

    if(!progPath.empty()) {
        VM vm(progPath);
        vm.run(printDecimal, dumpMem);
    }
    
    return 0;
}
