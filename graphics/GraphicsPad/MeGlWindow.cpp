#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <Primitives\Vertex.h>
using namespace std;

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 6;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
GLuint programID;
GLuint sizeofVerts;

float rotation = 0;
glm::vec2 position;

void sendDataToOpenGL()
{
	Vertex myTri[] =
	{
		glm::vec3(+0.0f, +0.0f, +1.0f),//0
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.5f, +0.0f, +1.0f),
		glm::vec3(+0.0f, +0.0f, +1.0f),

		glm::vec3(+0.7f, +0.7f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, +0.0f, +1.0f),
		glm::vec3(+0.0f, +0.0f, +1.0f),

		glm::vec3(+0.25f, -0.25f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.5f, +0.0f, +1.0f), //5
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, +0.0f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(-0.5f, +0.0f, +1.0f),
		glm::vec3(+0.0f, +0.0f, +1.0f),

		glm::vec3(-0.7f, -0.7f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, +0.0f, +1.0f),
		glm::vec3(+0.0f, +0.0f, +1.0f),

		glm::vec3(-0.25f, +0.25f, +1.0f), //10
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(-0.5f, +0.0f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, +0.0f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, +0.5f, +1.0f),
		glm::vec3(+0.0f, +0.0f, +1.0f),

		glm::vec3(-0.7f, +0.7f, +1.0f),
		glm::vec3(+0.93f, +0.9f, +0.52f),

		glm::vec3(+0.0f, +0.0f, +1.0f), //15
		glm::vec3(+0.93f, +0.57f, +0.13f),

		glm::vec3(+0.25f, +0.25f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, +0.5f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, +0.0f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, -0.5f, +1.0f),
		glm::vec3(+0.0f, +0.0f, +1.0f),

		glm::vec3(+0.7f, -0.7f, +1.0f), //20
		glm::vec3(+0.93f, +0.9f, +0.52f),

		glm::vec3(+0.0f, +0.0f, +1.0f),
		glm::vec3(+0.93f, +0.57f, +0.13f),

		glm::vec3(-0.25f, -0.25f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),

		glm::vec3(+0.0f, -0.5f, +1.0f),
		glm::vec3(+0.0f, +0.7f, +0.93f),
	};
	sizeofVerts = sizeof(myTri);
	//glm::mat3 transform = glm::mat3(glm::translate(0.5f, 0.0f, 0.0f));
	glm::mat3 transform;
	transform[2][0] = position.x;
	transform[2][1] = position.y;
	transform *= glm::mat3(glm::rotate(rotation, 0.0f, 0.0f, 1.0f));

	/*int numVerts = sizeof(myTri) / sizeof(*myTri);

	for (int i = 0; i < 3; i++)
	{
		myTri[i].position = transform * myTri[i].position;
	}*/

	GLint uniformLocation = glGetUniformLocation(programID, "transform");
	glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &transform[0][0]);

	GLuint vertexBufferID;
	GLushort indices[] = { 0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23};
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(myTri) + sizeof(indices), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(myTri), myTri);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(myTri), sizeof(indices), indices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));
}

void MeGlWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());
	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_SHORT, (void*)sizeofVerts);
}

void MeGlWindow::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		position.y += 0.01;
		sendDataToOpenGL();
		repaint();
		break;
	case Qt::Key::Key_S:
		position.y -= 0.01;
		sendDataToOpenGL();
		repaint();
		break;
	case Qt::Key::Key_A:
		position.x -= 0.01;
		sendDataToOpenGL();
		repaint();
		break;
	case Qt::Key::Key_D:
		position.x += 0.01;
		sendDataToOpenGL();
		repaint();
		break;
	case Qt::Key::Key_Q:
		rotation += 5;
		sendDataToOpenGL();
		repaint();
		break;
	case Qt::Key::Key_E:
		rotation -= 5;
		sendDataToOpenGL();
		repaint();
		break;
	}
}
bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if ( ! meInput.good())
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

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if( ! checkShaderStatus(vertexShaderID) || ! checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if ( ! checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

void MeGlWindow::initializeGL()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	installShaders();
	this->setFixedWidth(500);
	this->setFixedHeight(500);
	sendDataToOpenGL();
	
}