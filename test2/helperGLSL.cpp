#include "helperGLSL.h"

int readShaderSource(GLuint objectPointer, GLchar* file)
{
	FILE* fp;
	GLchar* source;
	GLsizei length;
	int ret;

	fp = fopen(file, "rb");
	if (fp == NULL)
	{
		perror(file);
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	length = ftell(fp);
	source = (GLchar*)malloc(length);

	if (source == NULL)
	{
		fprintf(stderr, "it could not allocate read buffer.\n");
		return -1;
	}
	fseek(fp, 0L, SEEK_SET);
	ret = fread((void*)source, 1, length, fp) != (size_t)length;
	fclose(fp);
	if (ret)
	{
		fprintf(stderr, "it could not read file.%s\n", file);
	}
	else
	{
		glShaderSource(objectPointer, 1, &source, &length);
	}
	free((void*)source);
	return ret;
}
void printShaderInfoLog(GLuint objectPointer)
{
	GLsizei bufsize;
	glGetShaderiv(objectPointer, GL_INFO_LOG_LENGTH, &bufsize);
	if (bufsize > 1)
	{
		GLchar* infoLog;
		infoLog = (GLchar*)malloc(bufsize);
		if (infoLog == NULL)
		{
			fprintf(stderr, "it could not allocate infoLog buffer.\n");
		}
		else
		{
			GLsizei length;
			glGetShaderInfoLog(objectPointer, bufsize, &length, infoLog);
		}
		free(infoLog);
	}
}
void printProgramInfoLog(GLuint objectPointer)
{
	GLsizei bufsize;
	glGetProgramiv(objectPointer, GL_INFO_LOG_LENGTH, &bufsize);
	if (bufsize > 1)
	{
		GLchar* infoLog;
		infoLog = (GLchar*)malloc(bufsize);
		if (infoLog == NULL)
		{
			fprintf(stderr, "it could not allocate infoLog buffer.\n");
		}
		else
		{
			GLsizei length;
			glGetProgramInfoLog(objectPointer, bufsize, &length, infoLog);
		}
		free(infoLog);
	}
}
void orthogonalMatrix(float left, float right,
	float bottom, float top,
	float near, float far,
	GLfloat *matrix)
{
	float dx = right - left;
	float dy = top - bottom;
	float dz = far - near;

	matrix[0] = 2.0f / dx;
	matrix[5] = 2.0f / dy;
	matrix[10] = -2.0f / dz;
	matrix[12] = -(right + left) / dx;
	matrix[13] = -(top + bottom) / dy;
	matrix[14] = -(far + near) / dz;
	matrix[15] = 1.0f;
	matrix[1] = matrix[2] = matrix[3] = matrix[4] =
		matrix[6] = matrix[7] = matrix[8] = matrix[9] = matrix[11] = 0.0f;
}

void perspectiveMatrix(float left, float right,
	float bottom, float top,
	float near, float far,
	GLfloat *matrix)
{
	float dx = right - left;
	float dy = top - bottom;
	float dz = far - near;

	matrix[0] = 2.0f * near / dx;
	matrix[5] = 2.0f * near / dy;
	matrix[8] = (right + left) / dx;
	matrix[9] = (top + bottom) / dy;
	matrix[10] = -(far + near) / dz;
	matrix[11] = -1.0f;
	matrix[14] = -2.0f * far * near / dz;
	matrix[1] = matrix[2] = matrix[3] = matrix[4] =
		matrix[6] = matrix[7] = matrix[12] = matrix[13] = matrix[15] = 0.0f;
}

void multiplyMatrix(const GLfloat *m0, const GLfloat *m1, GLfloat *matrix)
{
	for (int i = 0; i < 16; ++i) {
		int j = i & ~3, k = i & 3;

		matrix[i] = m0[j + 0] * m1[0 + k]
			+ m0[j + 1] * m1[4 + k]
			+ m0[j + 2] * m1[8 + k]
			+ m0[j + 3] * m1[12 + k];
	}
}

void lookAt(float ex, float ey, float ez,
	float tx, float ty, float tz,
	float ux, float uy, float uz,
	GLfloat *matrix)
{
	float l;
	tx = ex - tx;
	ty = ey - ty;
	tz = ez - tz;
	l = sqrtf(tx * tx + ty * ty + tz * tz);
	matrix[2] = tx / l;
	matrix[6] = ty / l;
	matrix[10] = tz / l;
	tx = uy * matrix[10] - uz * matrix[6];
	ty = uz * matrix[2] - ux * matrix[10];
	tz = ux * matrix[6] - uy * matrix[2];
	l = sqrtf(tx * tx + ty * ty + tz * tz);
	matrix[0] = tx / l;
	matrix[4] = ty / l;
	matrix[8] = tz / l;
	matrix[1] = matrix[6] * matrix[8] - matrix[10] * matrix[4];
	matrix[5] = matrix[10] * matrix[0] - matrix[2] * matrix[8];
	matrix[9] = matrix[2] * matrix[4] - matrix[6] * matrix[0];
	matrix[12] = -(ex * matrix[0] + ey * matrix[4] + ez * matrix[8]);
	matrix[13] = -(ex * matrix[1] + ey * matrix[5] + ez * matrix[9]);
	matrix[14] = -(ex * matrix[2] + ey * matrix[6] + ez * matrix[10]);
	matrix[3] = matrix[7] = matrix[11] = 0.0f;
	matrix[15] = 1.0f;
}
void cameraMatrix(float fovy, float aspect, float near, float far, GLfloat *matrix)
{
	float f = 1.0f / tanf(fovy * 0.5f * 3.141593f / 180.0f);
	float dz = far - near;

	matrix[0] = f / aspect;
	matrix[5] = f;
	matrix[10] = -(far + near) / dz;
	matrix[11] = -1.0f;
	matrix[14] = -2.0f * far * near / dz;
	matrix[1] = matrix[2] = matrix[3] = matrix[4] =
		matrix[6] = matrix[7] = matrix[8] = matrix[9] =
		matrix[12] = matrix[13] = matrix[15] = 0.0f;
}

GLuint solidFace(const GLfloat z_position, const GLuint *buffer)
{
	Position* position;
	Face* face;

	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Position) * 4, NULL, GL_STATIC_DRAW);
	position = (Position*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	position[0][0] = -1.0;
	position[0][1] = -1.0;
	position[0][2] = z_position;
	position[1][0] = 1.0;
	position[1][1] = -1.0;
	position[1][2] = z_position;
	position[2][0] = 1.0;
	position[2][1] = 1.0;
	position[2][2] = z_position;
	position[3][0] = -1.0;
	position[3][1] = 1.0;
	position[3][2] = z_position;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Face) * 2, NULL, GL_STATIC_DRAW);
	face = (Face*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	face[0][0] = 0;
	face[0][1] = 2;
	face[0][2] = 1;
	face[1][0] = 0;
	face[1][1] = 3;
	face[1][2] = 2;

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return 6;
}