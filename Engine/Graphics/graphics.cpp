#include "graphics.h"

Graphics::Graphics(SDL_Window* window) {
    // Specify shading technique
    glShadeModel(GL_SMOOTH);

    // Interpolation quality
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Initial color definition
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Check GLEW initialization
    GLenum error = glewInit();
    if(GLEW_OK != error) {
        // SDL_log("Error: %s\n", glewGetErrorString(error));
    } else {
        SDL_Log("GLEW Initialized Successfully");
        setViewport(SDL_GetWindowSurface(window) -> w, SDL_GetWindowSurface(window) -> h);
    }
}

Graphics::~Graphics() {

}

void Graphics::setViewport(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}