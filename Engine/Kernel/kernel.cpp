int Kernel::start() {
    // Check SDL configuration
    initSDL();

    // Create window instance
    SDL_Window* window = createWindow("Test", 600, 600);

    // Generate opengl context in window
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // Graphics manager
    Graphics* graphicsManager;
    graphicsManager = new Graphics(window);

    // Frame count
    int iFrame = 0;

    // Main loop
    bool isRunning = true;
    while (isRunning) {
        // iFrame increment
        iFrame += 1;

        // Update
        update();

        // Draw
        render(window, iFrame);
    }

    // Free resources
    cleanUp(window, glContext);
}

int Kernel::initSDL() {
    if (SDL_Init(SDL_INIT_NOPARACHUTE) && SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return -1;
    } else {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialized");
    }
}

SDL_Window* Kernel::createWindow(const char* windowTitle, int width, int height) {
    // Create instance
    SDL_Window* window = SDL_CreateWindow(
        windowTitle,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL
    );

    //Check that the window was succesfully created
    if(window == NULL)
    {
        //Print error, if null
        SDL_Log("Could not create window: %s\n", SDL_GetError());
    }
    else
        SDL_Log("Window Successful Generated");

    return window;
}

void Kernel::update() {

}

void Kernel::render(SDL_Window* window, int iFrame) {
    double theta = (double)iFrame/10000;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glBegin(GL_POLYGON);
    glColor3f(1, 0, 0); glVertex3f(-0.6*cos(theta)+0.75*sin(theta), -0.6*sin(theta)-0.75*cos(theta), 0.5);
    glColor3f(0, 1, 0); glVertex3f(0.6*cos(theta)+0.75*sin(theta), 0.6*sin(theta)-0.75*cos(theta), 0);
    glColor3f(0, 0, 1); glVertex3f(-0.75*sin(theta), 0.75*cos(theta), 0);
    glEnd();

    // Flush drawing command buffer to make drawing happen as soon as possible.
    glFlush();

    SDL_GL_SwapWindow(window);
}

void Kernel::cleanUp(SDL_Window* window, SDL_GLContext &glContext) {
    // Clean up resources
   SDL_GL_DeleteContext(glContext);
   SDL_DestroyWindow(window); 
} 