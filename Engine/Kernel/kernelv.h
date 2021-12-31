// Kernel for vector shader

#ifndef _KERNELV_H
#define _KERNELV_H

#include "../../common.h"

class KernelV {
    public:
        int start(int argc, char** argv);
    private:
        static bool isRunning;

        int initSDL();
        SDL_Window* createWindow(const char* windowTitle, int width, int height);
        void update();
        void render(SDL_Window* window, int iFrame);
        void cleanUp(SDL_Window* window, SDL_GLContext &glContext);
};

bool KernelV::isRunning = true;

#include "kernelv.cpp"

#endif