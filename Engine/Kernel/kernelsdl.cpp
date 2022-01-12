auto initT = chrono::steady_clock::now();

// data struct: (all unused are arbitrary)
//          (id #)
// [name]   [0]     [1]     [2]     [3]     [4]     [5]     [6]     [7]     [8]     [9]     [10]    [11]
// sphere   0       x       y       z       r                       red     green   blue    mat     refidx
// cone     1       x       y       z       c1      c2      h       red     green   blue    mat     refidx
// box      2       x       y       z       w       l       h       red     green   blue    mat     refidx
// r cyl	3		x		y		z		ra		rb		h       red     green   blue    mat     refidx
// mat (0 -> diffuse, 1 -> metal, 2 -> glass)
struct shader_data_t {
    float res[2] = {500, 500};
    int size = 3;
    int width = 12;
    float data[36] = {
        0,      0,      0,      0,      1,      0,      0,      0.2,      0.2,     0.2,       2 ,      1.5,
        0,      0,      -1001.5, 0,      1000,    0,      0,      0.2,      0.2,   0.2,     0,      0.0,
        //1,      0,      0,      2,      0.5,    0.5,    0.5,    0.5,    0,     0,       1
        2,      -1,     0,     4,      1,      1,      0.1,      0,      0.5,    0,      0,     0.0
        //3,      0,      -2,     0,      2,      0.5,    0.25,   0,      0,      0.5
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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialized");
    }
}

void Kernel::setShader() {
    string line, text;
    ifstream in("rayTracingShaderSrc.frag");
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

float* cross(float a[3], float b[3]) {
    float a1 = a[0]; float a2 = a[1]; float a3 = a[2];
    float b1 = b[0]; float b2 = b[1]; float b3 = b[2];

    static float r[3];
    r[0] = a2*b3-a3*b2;
    r[1] = a3*b1-a1*b3;
    r[2] = a1*b2-a2*b1;

    return r;
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
                    case SDLK_w: // w key
                        wDown = true;
                        break;
                    case SDLK_a: // a key
                        aDown = true;
                        break;
                    case SDLK_s: // s key
                        sDown = true;
                        break;
                    case SDLK_d: // d key
                        dDown = true;
                        break;
                    case SDLK_LSHIFT: // shift key
                        shDown = true;
                        break;
                    case SDLK_SPACE: // shift key
                        spDown = true;
                        break;
                    case SDLK_LCTRL: // left control key
                        ctrlDown = true;
                        break;
                    case SDLK_ESCAPE: // exit game
                        isRunning = false;
                        break;
                }
                break;

            case SDL_KEYUP:
                switch (m_event.key.keysym.sym) {
                    case SDLK_w: // w key
                        wDown = false;
                        break;
                    case SDLK_a: // a key
                        aDown = false;
                        break;
                    case SDLK_s: // s key
                        sDown = false;
                        break;
                    case SDLK_d: // d key
                        dDown = false;
                        break;
                    case SDLK_LSHIFT: // shift key
                        shDown = false;
                        break;
                    case SDLK_SPACE: // shift key
                        spDown = false;
                        break;
                    case SDLK_LCTRL: // left control key
                        ctrlDown = false;
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
            
            case SDL_MOUSEMOTION:
                int relx = m_event.motion.xrel;
                int rely = m_event.motion.yrel;
                //SDL_Log("%d, %d", relx, rely);
                float cX = 0.001;
                float cY = 0.001;
                curTheta += relx*cX;
                curPhi += rely*cY;

                if (curTheta > 2*PI) { curTheta -= 2*PI; }
                else if (curTheta < -2*PI) { curTheta += 2*PI; }
               
                if (curPhi > PI/2) { curPhi = PI/2 - 0.001; } 
                else if (curPhi < -PI/2) { curPhi = -PI/2 + 0.001; }

                setDir(curTheta, curPhi+PI/2);
                break;

        }
	}

    float moveC;
    if (ctrlDown) {
        moveC = 0.1;
    } else {
        moveC = 0.025;
    }
    if (wDown) {
        setPos(cameraRot[0]*moveC, cameraRot[1]*moveC, cameraRot[2]*moveC);
    }
    if (sDown) {
        setPos(-cameraRot[0]*moveC, -cameraRot[1]*moveC, -cameraRot[2]*moveC);
    }
    if (aDown) {
        setPos(cos(curTheta-PI/2)*moveC, 0, sin(curTheta-PI/2)*moveC);
    }
    if (dDown) {
        setPos(-cos(curTheta-PI/2)*moveC, 0, -sin(curTheta-PI/2)*moveC);
    }
    if (spDown) {
        setPos(0, moveC, 0);
    }
    if (shDown) {
        setPos(0, -moveC, 0);
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
    curTheta = -PI; curPhi = PI/4;
    setPos(5, 2, 0);
    setDir(curTheta, curPhi+PI/2);

    wDown = false;
    sDown = false;
    aDown = false;
    dDown = false;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    return window;
}

// add vector to position vector
void Kernel::setPos(float x, float y, float z) {
    cameraPos[0] += x;
    cameraPos[1] += y;
    cameraPos[2] += z;
}

void Kernel::setDir(float theta, float phi) {
    cameraRot[0] = sin(phi) * cos(theta);
    cameraRot[2] = sin(phi) * sin(theta);
    cameraRot[1] = cos(phi);
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
    glUniform3f(glGetUniformLocation(prog, "cPos"), cameraPos[0], cameraPos[1], cameraPos[2]);
    glUniform3f(glGetUniformLocation(prog, "cRot"), cameraRot[0], cameraRot[1], cameraRot[2]);

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