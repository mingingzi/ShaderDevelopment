#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
#include <glm\glm.hpp>
#include <Primitives\Vertex.h>
#include <Primitives\ShapeGenerator.h>
using namespace std;
using glm::vec3;

GLuint programID;
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
	tri.cleanup();
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>());
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const char* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	programID = glCreateProgram();
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
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());
	vec3 dominatingColor(1.0f, 0.0f, 0.0f);
	GLint dominatingColorUniformLocation = glGetUniformLocation(programID, "dominatingColor");
	GLint yFlipUniformLocation = glGetUniformLocation(programID, "yFlip");
	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform1f(yFlipUniformLocation, 1.0f);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)sizeofVerts);

	dominatingColor.r = 0;
	dominatingColor.b = 1;
	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform1f(yFlipUniformLocation, -1.0f);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)sizeofVerts);
}