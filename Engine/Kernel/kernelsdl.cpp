auto initT = chrono::steady_clock::now();

struct shader_data_t {
    float res[2] = {600, 600};
    int size = 2;
    int width = 7;
    float data[14] = {
        //0,      0,      0,      1,      1,      0,      0, 
        1,      0,      0,      2,      0.5,    0.5,    0.5,
        2,      -1,     -2,     2,      1,      1,      1
    };
} shader_data;

/**
 * Initialize and run the lifecycle of the open gl context
 * @return an int indicating the exit status of the program
 */
int Kernel::start(const char* windowTitle, int rx, int ry) {
    // Check SDL configuration
    initSDL();

    // Create window instance
    SDL_Window* window = createWindow(windowTitle, rx, ry);

    // Generate opengl context in window
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // Graphics manager
    Graphics* graphicsManager;
    graphicsManager = new Graphics(window);

    // Set shaders
    setShader();

    cout << "Setup Complete" << "\n";

    isRunning = true;
    // Main loop
    while (isRunning) {
        // Setup gl environment
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Background color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        // Update
        update();

        // Draw
        render(window);
        
        // Handle events
        events(window);
    }

    // Free resources
    cleanUp(window, glContext);
}

/**
 * Initialize SDL window and define OpenGL version (4.2)
 * @return int representing the exit state of the function
 */
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

void Kernel::setShader() {
    string line, text;
    ifstream in("shaderSrc.frag");
    while(getline(in, line))
    {
        text += line + "\n";
    }
    const char *f = text.c_str();

	const char *v =
		"varying float x, y, z;"
		"void main() {"
		"	gl_Position = ftransform();"
		"	x = gl_Position.x; y = gl_Position.y; z = gl_Position.z;"
		"}";
 
	vs = glCreateShader(GL_VERTEX_SHADER);
	ps = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ps, 1, &f, 0);
	glShaderSource(vs, 1, &v, 0);
 
	glCompileShader(vs);
	glCompileShader(ps);
 
	prog = glCreateProgram();
	glAttachShader(prog, ps);
	glAttachShader(prog, vs);
 
	glLinkProgram(prog);
	glUseProgram(prog);

    
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

/**
 * Handles events
 */
void Kernel::events(SDL_Window* window) {
    SDL_Event m_event;
	while(SDL_PollEvent(&m_event)) {
		switch (m_event.type) {
            case SDL_KEYDOWN:
                switch (m_event.key.keysym.sym) {
                    case SDLK_ESCAPE: // exit game
                        isRunning = false;
                        break;
                }
                break;

            case SDL_WINDOWEVENT:
                switch (m_event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE: // exit game
                        isRunning = false;
                        break;
                }
                break;
        }
	}
}

/**
 * Creates a window with the defined specifications
 * @param windowTitle title of the window
 * @param width width of the window in pixels (px)
 * @param height height of the window in pixels (px)
 * @return a pointer to an SDL_Window object with the assigned specifications, and a centered window position
 */
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
    if(window == NULL) {
        //Print error, if null
        SDL_Log("Could not create window: %s\n", SDL_GetError());
    } else
        SDL_Log("Window Successful Generated");

    resolution[0] = width; resolution[1] = height;

    return window;
}

void Kernel::update() {
    frame += 1;
    
    auto cur = chrono::steady_clock::now();
    std::chrono::duration<float> diff = cur - initT;
    float dt = diff.count() - curtime;
    curtime = diff.count();

    // system information
    cout << "\rFrame: " << frame << "\tTime: " << curtime << "\tdT: " << dt << "\tFPS: " << 1/dt;

    glClear(GL_COLOR_BUFFER_BIT);
	glUniform1i(iFrame, frame);
    glUniform1f(iTime, curtime);

    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    
    GLuint block_index = 0;
    block_index = glGetProgramResourceIndex(prog, GL_SHADER_STORAGE_BLOCK, "shader_data");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(p, &shader_data, sizeof(shader_data));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void Kernel::render(SDL_Window* window) {
    glBegin(GL_POLYGON);
		glVertex3f(-1, -1, 0);
		glVertex3f(-1, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, -1, 0);
	glEnd();
    
    // flush gl context to screen
    glFlush();

    SDL_GL_SwapWindow(window);
}

/**
 * Cleans up gl context and deletes the SDL window pane
 * @param window the SDL window to delete
 * @param glContext the open gl context to delete
 */
void Kernel::cleanUp(SDL_Window* window, SDL_GLContext &glContext) {
    // Clean up resources
   SDL_GL_DeleteContext(glContext);
   SDL_DestroyWindow(window); 
}