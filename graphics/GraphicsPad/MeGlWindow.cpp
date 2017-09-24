#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <Primitives\Vertex.h>
#include <Primitives\ShapeGenerator.h>
#include <chrono>

using namespace std;
using glm::vec3;
using glm::mat4;

GLuint programID;
GLuint sizeofVerts;
GLuint numIndices;
GLfloat yAngle = 0.0f;

void sendDataToOpenGL()
{
	ShapeData shape = ShapeGenerator::makeCube();

	sizeofVerts = shape.vertexBuffersize();

	GLuint myBufferID;
	
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);

	glBufferData(GL_ARRAY_BUFFER, shape.vertexBuffersize() + shape.indexBuffersize(), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, shape.vertexBuffersize(), shape.vertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, shape.vertexBuffersize(), shape.indexBuffersize(), shape.indices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));
	numIndices = shape.numIndices;
	shape.cleanup();
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
	glEnable(GL_DEPTH_TEST);
	sendDataToOpenGL();
	installShaders();
	this->setFixedWidth(800);
	this->setFixedHeight(500);

	//timer setup
	Mytimer = new QTimer(this);
	connect(Mytimer, SIGNAL(timeout()), this, SLOT(update()));
	Mytimer->setInterval(0);
	Mytimer->start();
}


void MeGlWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	//transformation
	mat4 projectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 10.0f);
	mat4 projectionTranslationMatrix = glm::translate(projectionMatrix, vec3(0.0f, 0.0f, -3.0f));
	mat4 xRotationMatrix = glm::rotate(projectionTranslationMatrix, 45.0f, vec3(1.0f, 0.0f, 0.0f));

	mat4 fullTransformMatrix = glm::rotate(xRotationMatrix, yAngle, vec3(-1.0f, -1.0f, 1.0f));

	GLint fullTransformMatrixMatrixUniformLocation = glGetUniformLocation(programID, "fullTransformMatrix");

	glUniformMatrix4fv(fullTransformMatrixMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, (void*)sizeofVerts);

	//rotate automatically with QTimer
	yAngle += 0.5f;
	if (yAngle > 360.0f) yAngle -= 360.0f;
}