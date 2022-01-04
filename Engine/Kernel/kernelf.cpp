int frame;
float curtime = 0;
auto initT = chrono::steady_clock::now();
GLuint ps, vs, prog, iFrame, iTime;
int resolution[2] = {600, 600};

// data struct: (all unused are arbitrary)
//          (id #)
// [name]   [0]     [1]     [2]     [3]     [4]     [5]     [6]     [7]     [8]
// sphere   0       x       y       z       r
// cone     1       x       y       z       c1      c2      h
// box      2       x       y       z       w       l       h
struct shader_data_t {
    float res[2] = {resolution[0], resolution[1]};
    int size = 2;
    int width = 7;
    float data[14] = {
        //0,      0,      0,      1,      1,      0,      0, 
        1,      0,      0,      2,      0.5,    0.5,    0.5,
        2,      -1,     -2,     2,      1,      1,      1
    };
} shader_data;
GLuint ssbo = 0;

void render(void) {
    auto cur = chrono::steady_clock::now();
    std::chrono::duration<float> diff = cur - initT;

    float dt = diff.count() - curtime;
    curtime = diff.count();
    
    frame += 1;
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

 
	glLoadIdentity();
	glBegin(GL_POLYGON);
		glVertex3f(-1, -1, 0);
		glVertex3f(-1, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, -1, 0);
	glEnd();
	glutSwapBuffers();
}

// We don't want the scene to get distorted when the window size changes, so
// we need a reshape callback.  We'll always maintain a range of -2.5..2.5 in
// the smaller of the width and height for our viewbox, and a range of -10..10
// for the viewbox depth.
void reshape(GLint w, GLint h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    GLfloat aspect = GLfloat(w) / GLfloat(h);
    glLoadIdentity();
    if (w <= h) {
        // width is smaller, so stretch out the height
        glOrtho(-2.5, 2.5, -2.5/aspect, 2.5/aspect, -10.0, 10.0);
    } else {
        // height is smaller, so stretch out the width
        glOrtho(-2.5*aspect, 2.5*aspect, -2.5, 2.5, -10.0, 10.0);
    }
}

void set_shader() {
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
 
int KernelF::start(int argc, char** argv) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);// | GLUT_MULTISAMPLE);
    glutInitWindowPosition(200, 120);
    glutInitWindowSize(resolution[0], resolution[1]);
    glutCreateWindow("Test");
    glutReshapeFunc(reshape);
	glutIdleFunc(render);
	glewInit();
    set_shader();
	glutMainLoop();
	return 0;
}