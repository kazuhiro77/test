#include "helperGLSL.h"

//test

GLuint vertShader;
GLuint fragShader;
GLuint gl2Program;

GLuint buffer[2];
GLfloat projectionMatrix[16];
GLint projectionMatrixLocation;

GLfloat tmp0[16], tmp1[16];
GLint points;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(gl2Program);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, projectionMatrix);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glDrawElements(GL_TRIANGLES, points, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glFlush();
}

void init(void)
{
	GLint compiled, linked;
	GLenum err = glewInit();
	GLfloat z_position = -1.0f;

	if (err != GLEW_OK)
	{
		fprintf(stderr, "GLEW:ERROR::%s\n", glewGetErrorString(err));
		exit(1);
	}
	glClearColor(0.3, 0.8, 0.3, 1.0);
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (readShaderSource(vertShader, "simple.vert"))
	{
		exit(1);
	}
	if (readShaderSource(fragShader, "simple.frag"))
	{
		exit(1);
	}
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
	printShaderInfoLog(vertShader);
	if (compiled == GL_FALSE)
	{
		fprintf(stderr, "vertex shader cannot be compiled.\n");
	}
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
	printShaderInfoLog(fragShader);
	if (compiled == GL_FALSE)
	{
		fprintf(stderr, "fragment shader cannot be compiled.\n");
	}
	gl2Program = glCreateProgram();
	glAttachShader(gl2Program, vertShader);
	glAttachShader(gl2Program, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glBindAttribLocation(gl2Program, 0, "position");

	glLinkProgram(gl2Program);
	glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
	printProgramInfoLog(gl2Program);
	if (linked == GL_FALSE)
	{
		fprintf(stderr, "program file cannot be linked.\n");
	}
	lookAt(4.0, 5.0, 6.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, tmp0);
	cameraMatrix(30.0f, 1.0f, 7.0f, 11.0f, tmp1);
	multiplyMatrix(tmp0, tmp1, projectionMatrix);
	projectionMatrixLocation = glGetUniformLocation(gl2Program, "projectionMatrix");
	glGenBuffers(2, buffer);
	points = solidFace(z_position, buffer);
}

//test program for github. creating a simple modern openGL program using glut

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(750, 750);
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow("test for github");
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return 0;
}