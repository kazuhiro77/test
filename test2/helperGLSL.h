#ifndef HELPERGLSL_H
#define HELPERGLSL_H

#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <glut.h>
#include "glext.h"
#include <math.h>

typedef float Position[3];
typedef GLuint Edge[2];
typedef GLuint Face[3];

int readShaderSource(GLuint objectPointer, GLchar* file);
void printShaderInfoLog(GLuint objectPointer);
void printProgramInfoLog(GLuint objectPointer);

void orthogonalMatrix(float left, float right, float bottom, float top, float near, float far, GLfloat *matrix);
void perspectiveMatrix(float left, float right, float bottom, float top, float near, float far, GLfloat *matrix);
void multiplyMatrix(const GLfloat *m0, const GLfloat *m1, GLfloat *matrix);
void lookAt(float ex, float ey, float ez, float tx, float ty, float tz, float ux, float uy, float uz, GLfloat *matrix);
void cameraMatrix(float fovy, float aspect, float near, float far, GLfloat *matrix);

GLuint solidFace(const GLfloat z_position, const GLuint *buffer);

#endif