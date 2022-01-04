// Kernel for SDL fragment shader

#ifndef _KERNEL_H
#define _KERNEL_H

#include "../../common.h"

class Kernel {
    public:
        int start(const char* windowTitle, int rx, int ry);
    private:
        bool isRunning = true;

        int initSDL();
        SDL_Window* createWindow(const char* windowTitle, int width, int height);
        void update();
        void render(SDL_Window* window);
        void cleanUp(SDL_Window* window, SDL_GLContext &glContext);
        void events(SDL_Window* window);
        void setShader();

        float frame, curtime = 0;
        GLuint ps, vs, prog, iFrame, iTime;
        GLuint ssbo = 0;
        int resolution[2];

};

#include "kernelsdl.cpp"

#endif