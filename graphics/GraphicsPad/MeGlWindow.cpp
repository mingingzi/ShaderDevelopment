#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <QtGui\qmouseevent>
#include <QtGui\qkeyevent>
#include <MeGlWindow.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <Primitives\Vertex.h>
#include <Primitives\ShapeGenerator.h>
#include <chrono>
#include "Camera.h"

using namespace std;
using glm::vec3;
using glm::mat4;

GLuint programID;
GLuint lightProgramID;

GLuint cubeSizeofVerts;
GLuint planeSizeofVerts;
GLuint sphereSizeofVerts;

GLuint cubeVertexArrayObjectID;
GLuint planeVertexArrayObjectID;
GLuint sphereVertexArrayObjectID;


GLuint cubeNumIndices;
GLuint planeNumIndices;
GLuint sphereNumIndices;


GLfloat yAngle = 0.0f;
Camera camera;
glm::vec3 lightPosition(0.0f, 0.0f, -4.0f);

void sendDataToOpenGL()
{
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData plane = ShapeGenerator::makePlane(15);
	ShapeData sphere = ShapeGenerator::makeSphere(20);

	GLuint myBufferID;
	
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);

	glBufferData(GL_ARRAY_BUFFER, 
		cube.vertexBuffersize() + cube.indexBuffersize() + 
		plane.vertexBuffersize() + plane.indexBuffersize() + 
		sphere.vertexBuffersize() + sphere.indexBuffersize(), 0, GL_STATIC_DRAW);

	GLsizeiptr currentOffset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.vertexBuffersize(), cube.vertices);
	currentOffset += cube.vertexBuffersize();
	cubeSizeofVerts = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, cube.indexBuffersize(), cube.indices);
	currentOffset += cube.indexBuffersize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.vertexBuffersize(), plane.vertices);
	currentOffset += plane.vertexBuffersize();
	planeSizeofVerts = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, plane.indexBuffersize(), plane.indices);
	currentOffset += plane.indexBuffersize();
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.vertexBuffersize(), sphere.vertices);
	currentOffset += sphere.vertexBuffersize();
	sphereSizeofVerts = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, sphere.indexBuffersize(), sphere.indices);
	currentOffset += sphere.indexBuffersize();
	
	cubeNumIndices = cube.numIndices;
	planeNumIndices = plane.numIndices;
	sphereNumIndices = sphere.numIndices;
	
	glGenVertexArrays(1, &cubeVertexArrayObjectID);
	glGenVertexArrays(1, &planeVertexArrayObjectID);
	glGenVertexArrays(1, &sphereVertexArrayObjectID);

	glBindVertexArray(cubeVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(sizeof(float) * 6));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(sizeof(float) * 9));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);

	glBindVertexArray(planeVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	GLuint planeByteOffset = cube.vertexBuffersize() + cube.indexBuffersize();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(planeByteOffset));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(planeByteOffset + sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(planeByteOffset + sizeof(float) * 6));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(planeByteOffset + sizeof(float) * 9));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);

	glBindVertexArray(sphereVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	GLuint sphereByteOffset = planeByteOffset + plane.vertexBuffersize() + plane.indexBuffersize();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(sphereByteOffset));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(sphereByteOffset + sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(sphereByteOffset + sizeof(float) * 6));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (char*)(sphereByteOffset + sizeof(float) * 9));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);

	cube.cleanup();
	plane.cleanup();
	sphere.cleanup();
}

void MeGlWindow::mouseMoveEvent(QMouseEvent* e)
{
	camera.mouseUpdate(glm::vec2(e->x(), e->y()));
	repaint();
}

void MeGlWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		camera.moveForward();
		break;
	case Qt::Key::Key_S:
		camera.moveBackward();
		break;
	case Qt::Key::Key_A:
		camera.strafeLeft();
		break;
	case Qt::Key::Key_D:
		camera.strafeRight();
		break;
	case Qt::Key::Key_R:
		camera.moveUp();
		break;
	case Qt::Key::Key_F:
		camera.moveDown();
		break;
	case Qt::Key::Key_I:
		lightPosition += glm::vec3(0, 0, -0.2);
		break;
	case Qt::Key::Key_K:
		lightPosition += glm::vec3(0, 0, 0.2);
		break;
	case Qt::Key::Key_Left:
		lightPosition += glm::vec3(-0.2, 0, -0.0);
		break;
	case Qt::Key::Key_Right:
		lightPosition += glm::vec3(0.2, 0, -0.0);
		break;
	case Qt::Key::Key_Up:
		lightPosition += glm::vec3(0, 0.2, -0.0);
		break;
	case Qt::Key::Key_Down:
		lightPosition += glm::vec3(0, -0.2, -0.0);
	}
	repaint();
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

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// Light Placeholder Shader
	temp = readShaderCode("LightVertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("LightFragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	lightProgramID = glCreateProgram();
	glAttachShader(lightProgramID, vertexShaderID);
	glAttachShader(lightProgramID, fragmentShaderID);
	glLinkProgram(lightProgramID);
}


void MeGlWindow::initializeGL()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	sendDataToOpenGL();
	installShaders();
	this->setFixedWidth(1600);
	this->setFixedHeight(1000);

	//Timer Setup
	Mytimer = new QTimer(this);
	connect(Mytimer, SIGNAL(timeout()), this, SLOT(update()));
	Mytimer->setInterval(0);
	Mytimer->start();
}


void MeGlWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	// Change with QTimer
	yAngle += 0.5f;
	if (yAngle > 360.0f) yAngle -= 360.0f;

	// Matrix Setup
	glUseProgram(programID);
	glm::mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	glm::mat4 viewToProjectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 20.0f);
	glm::mat4 World2ProjectionMatrix = viewToProjectionMatrix  * worldToViewMatrix;

	GLint fullTransformMatrixMatrixUniformLocation = glGetUniformLocation(programID, "fullTransformMatrix");
	GLint modelToWorldMatrixUniformLocation = glGetUniformLocation(programID, "modelToWorldMatrix");

	// Lighting Setup
	glm::vec4 ambientLight(0.2f, 0.2f, 0.2f, 1.0f);
	
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	// Camera Setup
	GLint eyePositionWorldUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	glm::vec3 eyePosition = camera.getPosition();
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);

	// Texture Setup
	const char* texName = "blueBird.png";
	QImage texture = QGLWidget::convertToGLFormat(QImage(texName, "PNG"));
	glActiveTexture(GL_TEXTURE0);
	GLuint textureBufferID;
	glGenTextures(1, &textureBufferID);
	glBindTexture(GL_TEXTURE_2D, textureBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLint textureUniformLocation = glGetUniformLocation(programID, "myTexture");
	glUniform1i(textureUniformLocation, 0);

	// Cube1
	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cubeModelToWorldMatrix = 
		glm::translate(-3.0f, 0.5f, -8.0f) *
		glm::rotate(45.0f, vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(yAngle, vec3(-1.0f, -1.0f, 1.0f));
	mat4 fullTransformMatrix = World2ProjectionMatrix * cubeModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformMatrixMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &cubeModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeofVerts);

	// Sphere1
	glBindVertexArray(sphereVertexArrayObjectID);
	mat4 sphereModelToWorldMatrix = glm::translate(1.0f, 0.0f, -8.0f);
	fullTransformMatrix = World2ProjectionMatrix * sphereModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformMatrixMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &sphereModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereSizeofVerts);

	// Base Plane
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(planeVertexArrayObjectID);
	mat4 planeModelToWorldMatrix =
		glm::translate(0.0f, 0.0f, -10.0f) *
		glm::rotate(30.0f, vec3(1.0f, 0.0f, 0.0f));
	fullTransformMatrix = World2ProjectionMatrix * planeModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformMatrixMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &planeModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, (void*)planeSizeofVerts);

	// Light Placeholder
	glUseProgram(lightProgramID);
	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 lightModelToWorldMatrix =
		glm::translate(lightPosition) * 
		glm::scale(0.08f, 0.08f, 0.08f);
	GLuint LightTransformMatrixUniformLocation = glGetUniformLocation(lightProgramID, "lightTransformMatrix");
	mat4 lightTransformMatrix = World2ProjectionMatrix  *  lightModelToWorldMatrix;
	glUniformMatrix4fv(LightTransformMatrixUniformLocation, 1, GL_FALSE, &lightTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeofVerts);
}