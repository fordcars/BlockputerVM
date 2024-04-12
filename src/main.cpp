#include "getopt.h"
#include <iostream>

#include "VM.hpp"

void printHelp(const std::string& progName) {
    std::cout
        << "Usage:\n"
        << "    " << progName << " OPTIONS\n"
        << "Options:\n"
        << "    -i        input file"
        << std::endl;
}

int main(int argc, char** argv) {
    int c;
    std::string progPath;
    while((c = getopt(argc, argv, "hi:")) != -1) {
        switch(c) {
            case 'i':
            {
                progPath = optarg;
                break;
            }
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
        vm.run();
    }
    
    return 0;
}
