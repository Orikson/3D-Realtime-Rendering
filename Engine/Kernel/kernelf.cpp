int frame;
GLuint ps, vs, prog, iFrame;

struct shader_data_t {
    float data[4] = {0.33, 0.33, 0.66, 1.0};
} shader_data;
GLuint ssbo = 0;

void render(void) {
    frame += 1;
    cout << "\rFrame: " << frame;
	glClear(GL_COLOR_BUFFER_BIT);
	glUniform1f(iFrame, frame);

    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    
    GLuint block_index = 0;
    block_index = glGetProgramResourceIndex(prog, GL_SHADER_STORAGE_BLOCK, "shader_data");


    shader_data.data[0] = 0.33;
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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition(80, 80);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Test");
    glutReshapeFunc(reshape);
	glutIdleFunc(render);
	glewInit();
    set_shader();
	glutMainLoop();
	return 0;
}