#include "Engine/Kernel/kernelf.h"
#include "Engine/Kernel/kernelv.h"

int main(int argc, char* argv[])
{
    KernelF* kernel;
    kernel->start(argc, argv); 

    return 0;
}