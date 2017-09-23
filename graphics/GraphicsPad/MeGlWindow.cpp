#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
#include <glm\glm.hpp>
#include <Primitives\Vertex.h>
#include <Primitives\ShapeGenerator.h>


extern const char* vertexShaderCode;
extern const char* fragmentShaderCode;

GLuint sizeofVerts;
void sendDataToOpenGL()
{
	ShapeData tri = ShapeGenerator::makeTriangle();

	sizeofVerts = tri.vertexBuffersize();

	GLuint myBufferID;
	
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);

	glBufferData(GL_ARRAY_BUFFER, tri.vertexBuffersize() + tri.indexBuffersize(), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, tri.vertexBuffersize(), tri.vertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, tri.vertexBuffersize(), tri.indexBuffersize(), tri.indices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const char* adapter[1];
	adapter[0] = vertexShaderCode;
	glShaderSource(vertexShaderID, 1, adapter, 0);
	adapter[0] = fragmentShaderCode;
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glUseProgram(programID);

}

void MeGlWindow::initializeGL()
{
	glewInit();
	sendDataToOpenGL();
	installShaders();
	this->setFixedWidth(500);
	this->setFixedHeight(500);
}

void MeGlWindow::paintGL()
{
	glViewport(0, 0, width(), height());
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)sizeofVerts);

}