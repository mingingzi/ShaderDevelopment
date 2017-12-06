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
GLuint planeTextureProgramID;
GLuint cubemapProgramID;
GLuint rendertoTextureProgramID;
GLuint receiveShadowProgramID;
GLuint edgeDetectionProgramID;


GLuint cubeSizeofVerts;
GLuint planeSizeofVerts;
GLuint sphereSizeofVerts;

GLuint cubeVertexArrayObjectID;
GLuint planeVertexArrayObjectID;
GLuint sphereVertexArrayObjectID;


GLuint cubeNumIndices;
GLuint planeNumIndices;
GLuint sphereNumIndices;

GLuint fboHandle;
GLuint renderTex;
GLuint frameDepthID;

GLfloat yAngle = 0.0f;
Camera camera;
Camera lightCamera;

glm::vec3 lightPosition(0.0f, 0.0f, 0.0f);
GLfloat fresnelScale = 0.5f;
// bottom and top are switched
const char* MeGlWindow::TexFile[] = { "texture/right.png","texture/left.png","texture/bottom.png","texture/top.png","texture/back.png","texture/front.png" };

void textureSetup() {
	// Texture cube
	const char* texName = "texture/blueBird.png";
	QImage texture = QGLWidget::convertToGLFormat(QImage(texName, "PNG"));
	glActiveTexture(GL_TEXTURE0);
	GLuint textureBufferID;
	glGenTextures(1, &textureBufferID);
	glBindTexture(GL_TEXTURE_2D, textureBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Texture sphere
	texName = "texture/Glass_Diffuse.png";
	texture = QGLWidget::convertToGLFormat(QImage(texName, "PNG"));
	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &textureBufferID);
	glBindTexture(GL_TEXTURE_2D, textureBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Normal cube
	const char* normalMapName = "texture/Shapes.png";
	QImage Normalmap = QGLWidget::convertToGLFormat(QImage(normalMapName, "PNG"));
	glActiveTexture(GL_TEXTURE1);
	GLuint NormalBufferID;
	glGenTextures(1, &NormalBufferID);
	glBindTexture(GL_TEXTURE_2D, NormalBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Normalmap.width(), Normalmap.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Normalmap.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Normal sphere
	normalMapName = "texture/Shapes.png";
	Normalmap = QGLWidget::convertToGLFormat(QImage(normalMapName, "PNG"));
	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &NormalBufferID);
	glBindTexture(GL_TEXTURE_2D, NormalBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Normalmap.width(), Normalmap.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Normalmap.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void MeGlWindow::shadowSetup()
{
	glActiveTexture(GL_TEXTURE6); // Use texture unit 6
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_DEPTH_ATTACHMENT);

	// Create the texture object
	glGenTextures(1, &renderTex);
	glBindTexture(GL_TEXTURE_2D, renderTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE5);
	glGenTextures(1, &frameDepthID);
	glBindTexture(GL_TEXTURE_2D, frameDepthID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width(), height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sizeof(float) * 6));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sizeof(float) * 9));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sizeof(float) * 11));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);

	glBindVertexArray(planeVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	GLuint planeByteOffset = cube.vertexBuffersize() + cube.indexBuffersize();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(planeByteOffset));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(planeByteOffset + sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(planeByteOffset + sizeof(float) * 6));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(planeByteOffset + sizeof(float) * 9));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(planeByteOffset + sizeof(float) * 11));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);

	glBindVertexArray(sphereVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	GLuint sphereByteOffset = planeByteOffset + plane.vertexBuffersize() + plane.indexBuffersize();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sphereByteOffset));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sphereByteOffset + sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sphereByteOffset + sizeof(float) * 6));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sphereByteOffset + sizeof(float) * 9));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (char*)(sphereByteOffset + sizeof(float) * 11));
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

	// Cubemap Shader
	temp = readShaderCode("shader/CubemapVertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("shader/CubemapFragmentShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	cubemapProgramID = glCreateProgram();
	glAttachShader(cubemapProgramID, vertexShaderID);
	glAttachShader(cubemapProgramID, fragmentShaderID);
	glLinkProgram(cubemapProgramID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// Light Placeholder Shader
	temp = readShaderCode("shader/LightVertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("shader/LightFragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	lightProgramID = glCreateProgram();
	glAttachShader(lightProgramID, vertexShaderID);
	glAttachShader(lightProgramID, fragmentShaderID);
	glLinkProgram(lightProgramID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// Plane Texture Shader
	temp = readShaderCode("PlaneTextureVertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("PlaneTextureFragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	planeTextureProgramID = glCreateProgram();
	glAttachShader(planeTextureProgramID, vertexShaderID);
	glAttachShader(planeTextureProgramID, fragmentShaderID);
	glLinkProgram(planeTextureProgramID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// Shadow Map Shader
	temp = readShaderCode("shader/RendertoTextureVertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("shader/RendertoTextureFragmentShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	rendertoTextureProgramID = glCreateProgram();
	glAttachShader(rendertoTextureProgramID, vertexShaderID);
	glAttachShader(rendertoTextureProgramID, fragmentShaderID);
	glLinkProgram(rendertoTextureProgramID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// Receive Shadow Shader
	temp = readShaderCode("shader/ReceiveShadowVertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("shader/ReceiveShadowFragmentShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	receiveShadowProgramID = glCreateProgram();
	glAttachShader(receiveShadowProgramID, vertexShaderID);
	glAttachShader(receiveShadowProgramID, fragmentShaderID);
	glLinkProgram(receiveShadowProgramID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// Edge Detection Shader
	temp = readShaderCode("shader/EdgeDetectionVertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("shader/EdgeDetectionFragmentShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	edgeDetectionProgramID = glCreateProgram();
	glAttachShader(edgeDetectionProgramID, vertexShaderID);
	glAttachShader(edgeDetectionProgramID, fragmentShaderID);
	glLinkProgram(edgeDetectionProgramID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

void MeGlWindow::loadCubemap() {
	
	glActiveTexture(GL_TEXTURE2);
	GLuint CubeBufferID;
	glGenTextures(1, &CubeBufferID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeBufferID);
	for (int i = 0; i < 6; ++i) {
		QImage Texdata = QGLWidget::convertToGLFormat(QImage(TexFile[i], "PNG"));
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, Texdata.width(), Texdata.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Texdata.bits());
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void MeGlWindow::initializeGL()
{
	this->setFixedWidth(1600);
	this->setFixedHeight(1000);
	glewInit();
	glEnable(GL_DEPTH_TEST);
	textureSetup();
	shadowSetup();
	sendDataToOpenGL();
	installShaders();
	loadCubemap();
	

	//Timer Setup
	Mytimer = new QTimer(this);
	connect(Mytimer, SIGNAL(timeout()), this, SLOT(update()));
	Mytimer->setInterval(0);
	Mytimer->start();
}


void MeGlWindow::paintGL()
{
	// Bind to texture's FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameDepthID, 0);

	lightCamera.setPosition(lightPosition);
	renderCamera(lightCamera, programID);

	// Unbind texture's FBO (back to default FB)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width(), height()); // Viewport for main window
	GLuint status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);
	//renderCamera(lightCamera, programID);
	renderCamera(camera, programID);

	// Scene matrix setup
	glm::mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	glm::mat4 viewToProjectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 140.0f);
	glm::mat4 World2ProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix;
	GLint fullTransformMatrixMatrixUniformLocation = glGetUniformLocation(rendertoTextureProgramID, "fullTransformMatrix");

	// Use the render texture to draw a cube
	glUseProgram(rendertoTextureProgramID);
	GLuint rendertoTextureUniformLocation = glGetUniformLocation(rendertoTextureProgramID, "myTexture");
	glUniform1i(rendertoTextureUniformLocation, 6);

	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cubeModelToWorldMatrix =
		glm::translate(0.0f, 2.0f, -15.0f);
	mat4 fullTransformMatrix = World2ProjectionMatrix * cubeModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformMatrixMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeofVerts);

	// Base Plane(Shader2)
	glUseProgram(receiveShadowProgramID);
	// Lighting Setup
	glm::vec4 ambientLight = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);

	GLuint ambientLightUniformLocation = glGetUniformLocation(receiveShadowProgramID, "ambientLight");
	glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	GLuint lightPositionUniformLocation = glGetUniformLocation(receiveShadowProgramID, "lightPositionWorld");
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	glm::mat4 LightWorld2ProjectionMatrix = viewToProjectionMatrix * lightCamera.getWorldToViewMatrix();
	GLint LightFullTransformMatrixUniformLocaiton = glGetUniformLocation(receiveShadowProgramID, "LightFullTransformMatrix");
	glUniformMatrix4fv(LightFullTransformMatrixUniformLocaiton, 1, GL_FALSE, &LightWorld2ProjectionMatrix[0][0]);

	// Camera Setup
	glm::vec3 eyePosition = camera.getPosition();
	GLuint eyePositionWorldUniformLocation = glGetUniformLocation(receiveShadowProgramID, "eyePositionWorld");
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);

	GLuint shadowMapUniformLocation = glGetUniformLocation(receiveShadowProgramID, "shadowMap");
	glUniform1i(shadowMapUniformLocation, 5);

	glBindVertexArray(planeVertexArrayObjectID);
	mat4 planeModelToWorldMatrix =
		glm::translate(0.0f, -1.0f, -15.0f) *
		glm::rotate(10.0f, vec3(1.0f, 0.0f, 0.0f));
	GLuint planeTextureTransformMatrixUniformLocation = glGetUniformLocation(receiveShadowProgramID, "fullTransformMatrix");
	GLuint planeTextureM2WMatrixUniformLocation = glGetUniformLocation(receiveShadowProgramID, "planeM2WTransformMatrix");
	mat4 planeTextureTransformMatrix = World2ProjectionMatrix * planeModelToWorldMatrix;
	glUniformMatrix4fv(planeTextureTransformMatrixUniformLocation, 1, GL_FALSE, &planeTextureTransformMatrix[0][0]);
	glUniformMatrix4fv(planeTextureM2WMatrixUniformLocation, 1, GL_FALSE, &planeModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, (void*)planeSizeofVerts);

	// CubeMap Setup
	GLint CubeMapUniformLocation = glGetUniformLocation(programID, "cubemap");
	glUniform1i(CubeMapUniformLocation, 2);

	// CubeMap
	glUseProgram(cubemapProgramID);
	CubeMapUniformLocation = glGetUniformLocation(cubemapProgramID, "cubemap");
	glUniform1i(CubeMapUniformLocation, 2);

	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cubemapModelToWorldMatrix = glm::scale(70.0f, 70.0f, 70.0f);
	// Cubemap doesn't move with camera
	worldToViewMatrix[3][0] = 0.0;
	worldToViewMatrix[3][1] = 0.0;
	worldToViewMatrix[3][2] = 0.0;
	GLuint SkyboxTransformMatrixUniformLocation = glGetUniformLocation(cubemapProgramID, "skyboxTransformMatrix");
	mat4 skyboxTransformMatrix = viewToProjectionMatrix * worldToViewMatrix * cubemapModelToWorldMatrix;
	glUniformMatrix4fv(SkyboxTransformMatrixUniformLocation, 1, GL_FALSE, &skyboxTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeofVerts);

	//PostProcess Plane
	glUseProgram(edgeDetectionProgramID);
	rendertoTextureUniformLocation = glGetUniformLocation(edgeDetectionProgramID, "RenderTex");
	glUniform1i(rendertoTextureUniformLocation, 6);

	glBindVertexArray(planeVertexArrayObjectID);
	planeModelToWorldMatrix =
		glm::translate(-0.5f, 0.8f, -7.0f) *
		glm::rotate(90.0f, vec3(1.0f, 0.0f, 0.0f)) *
		glm::scale(0.7f, 0.7f, 0.7f);
	planeTextureTransformMatrixUniformLocation = glGetUniformLocation(edgeDetectionProgramID, "fullTransformMatrix");
	planeTextureM2WMatrixUniformLocation = glGetUniformLocation(edgeDetectionProgramID, "modelToWorldMatrix");
	planeTextureTransformMatrix = World2ProjectionMatrix * planeModelToWorldMatrix;
	glUniformMatrix4fv(planeTextureTransformMatrixUniformLocation, 1, GL_FALSE, &planeTextureTransformMatrix[0][0]);
	glUniformMatrix4fv(planeTextureM2WMatrixUniformLocation, 1, GL_FALSE, &planeModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, (void*)planeSizeofVerts);
}

void MeGlWindow::renderCamera(Camera &camera, GLuint programID) {
	// Change with QTimer
	yAngle += 0.5f;
	if (yAngle > 360.0f) yAngle -= 360.0f;

	// Matrix Setup
	glm::mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	glm::mat4 viewToProjectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 20.0f);
	glm::mat4 World2ProjectionMatrix = viewToProjectionMatrix  * worldToViewMatrix;

	GLint fullTransformMatrixMatrixUniformLocation = glGetUniformLocation(programID, "fullTransformMatrix");
	GLint modelToWorldMatrixUniformLocation = glGetUniformLocation(programID, "modelToWorldMatrix");


	// Major Shader (Shader1)
	glUseProgram(programID);
	// Lighting Setup
	glm::vec4 ambientLight = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	GLint fresnelScaleUniformLocation = glGetUniformLocation(programID, "fresnelScale");
	glUniform1f(fresnelScaleUniformLocation, fresnelScale);


	// Camera Setup
	GLint eyePositionWorldUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	glm::vec3 eyePosition = camera.getPosition();
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);


	// Texture
	GLint textureUniformLocation = glGetUniformLocation(programID, "myTexture");
	glUniform1i(textureUniformLocation, 0);
	// Normal Setup
	GLint normalmapUniformLocation = glGetUniformLocation(programID, "normalMap");
	glUniform1i(normalmapUniformLocation, 1);
	// CubeMap Setup
	GLint CubeMapUniformLocation = glGetUniformLocation(programID, "cubemap");
	glUniform1i(CubeMapUniformLocation, 2);
	GLuint rendertoTextureUniformLocation = glGetUniformLocation(programID, "RenderTex");
	glUniform1i(rendertoTextureUniformLocation, 6);

	// Cube1
	glBindVertexArray(cubeVertexArrayObjectID);
	mat4 cubeModelToWorldMatrix =
		glm::translate(-3.0f, 0.5f, -8.0f) *
		glm::rotate(45.0f, vec3(1.0f, 0.0f, 0.0f));
	/*glm::rotate(45.0f, vec3(1.0f, 0.0f, 0.0f)) *
	glm::rotate(yAngle, vec3(-1.0f, -1.0f, 1.0f));*/
	mat4 fullTransformMatrix = World2ProjectionMatrix * cubeModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformMatrixMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &cubeModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, (void*)cubeSizeofVerts);

	// Sphere1
	// texture
	glUniform1i(textureUniformLocation, 3);
	// Normal Setup
	glUniform1i(normalmapUniformLocation, 4);

	glBindVertexArray(sphereVertexArrayObjectID);
	mat4 sphereModelToWorldMatrix = glm::translate(1.0f, 0.0f, -8.0f) *
		glm::rotate(yAngle, vec3(-1.0f, -1.0f, 1.0f));
	fullTransformMatrix = World2ProjectionMatrix * sphereModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformMatrixMatrixUniformLocation, 1, GL_FALSE, &fullTransformMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &sphereModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, sphereNumIndices, GL_UNSIGNED_SHORT, (void*)sphereSizeofVerts);


	// Base Plane(Shader2)
	glUseProgram(planeTextureProgramID);
	// Lighting Setup
	ambientLightUniformLocation = glGetUniformLocation(planeTextureProgramID, "ambientLight");
	glUniform4fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	lightPositionUniformLocation = glGetUniformLocation(planeTextureProgramID, "lightPositionWorld");
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	// Camera Setup
	eyePositionWorldUniformLocation = glGetUniformLocation(planeTextureProgramID, "eyePositionWorld");
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);

	glBindVertexArray(planeVertexArrayObjectID);
	mat4 planeModelToWorldMatrix =
		glm::translate(0.0f, -1.0f, -15.0f) *
		glm::rotate(10.0f, vec3(1.0f, 0.0f, 0.0f));
	GLuint planeTextureTransformMatrixUniformLocation = glGetUniformLocation(planeTextureProgramID, "planeTextureTransformMatrix");
	GLuint planeTextureM2WMatrixUniformLocation = glGetUniformLocation(planeTextureProgramID, "planeModelToWorldMatrix");
	mat4 planeTextureTransformMatrix = World2ProjectionMatrix * planeModelToWorldMatrix;
	glUniformMatrix4fv(planeTextureTransformMatrixUniformLocation, 1, GL_FALSE, &planeTextureTransformMatrix[0][0]);
	glUniformMatrix4fv(planeTextureM2WMatrixUniformLocation, 1, GL_FALSE, &planeModelToWorldMatrix[0][0]);
	//glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, (void*)planeSizeofVerts);



	// Light Placeholder(Shader3)
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