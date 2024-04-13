#include "getopt.h"
#include <iostream>

#include "VM.hpp"

void printHelp(const std::string& progName) {
    std::cout
        << "Usage:\n"
        << "    " << progName << " OPTIONS\n"
        << "Options:\n"
        << "    -i        input file\n"
        << "    -m        dump memory after execution"
        << std::endl;
}

int main(int argc, char** argv) {
    int c;
    std::string progPath;
    bool dumpMem = false;
    while((c = getopt(argc, argv, "hi:m")) != -1) {
        switch(c) {
            case 'i':
            {
                progPath = optarg;
                break;
            }
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
        vm.run(dumpMem);
    }
    
    return 0;
}
