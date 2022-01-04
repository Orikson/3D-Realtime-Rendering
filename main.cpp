//#include "Engine/Kernel/kernelf.h"
//#include "Engine/Kernel/kernelv.h"
#include "Engine/Kernel/kernelsdl.h"

int main(int argc, char* argv[])
{
    Kernel* kernel;
    kernel->start("3D Rendering", 600, 600);//argc, argv); 

    return 0;
}