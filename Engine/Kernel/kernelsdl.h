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
        void setPos(float x, float y, float z);
        void setDir(float theta, float phi);

        float cameraPos[3];
        float cameraRot[3];
        float curTheta;
        float curPhi;

        bool wDown;
        bool sDown;
        bool aDown;
        bool dDown;
        bool spDown;
        bool shDown;
        bool ctrlDown;

        float frame, curtime = 0;
        GLuint ps, vs, prog, iFrame, iTime;
        GLint cPos, cRot;
        GLuint ssbo = 0;
        int resolution[2];

};

#include "kernelsdl.cpp"

#endif