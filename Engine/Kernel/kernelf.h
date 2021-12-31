// Kernel for fragment shader

#ifndef _KERNELV_H
#define _KERNELV_H

#include "../../common.h"

class KernelF {
    public:
        int start(int argc, char** argv);
    private:
        static bool isRunning;        
};

bool KernelF::isRunning = true;

#include "kernelf.cpp"

#endif