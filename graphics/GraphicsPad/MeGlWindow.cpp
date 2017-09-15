#include <gl\glew.h>
#include <MeGlWindow.h>

extern const char* vertexShaderCode;
extern const char* fragmentShaderCode;

GLuint sizeofVerts;
void sendDataToOpenGL()
{
	GLfloat verts[] =
	{
		+0.0f, +0.0f,//0
		+0.93f, +0.71f, +0.13f,
		+1.0f, +1.0f,//1
		+0.93f, +0.71f, +0.13f,
		-1.0f, +1.0f,//2
		+0.93f, +0.71f, +0.13f,
		+0.0f, +0.0f,//3
		+0.0f, +0.98f, +0.6f,
		-1.0f, +1.0f,//4
		+0.0f, +0.98f, +0.6f,
		-1.0f, -1.0f,//5
		+0.0f, +0.98f, +0.6f,
		+0.0f, +0.0f,//6
		+0.0f, +0.0f, +1.0f,
		-0.5f, -0.5f,//7
		+1.0f, +0.0f, +0.0f,
		+0.5f, -0.5f,//8
		+0.0f, +1.0f, +0.0f,
		+0.0f, -1.0f,//9
		+0.537f, +0.812f, +0.941f,
		+0.5f, -0.5f,//10
		+0.537f, +0.812f, +0.941f,
		-0.5f, -0.5f,//11
		+0.537f, +0.812f, +0.941f,
		-1.0f, -1.0f,//12
		+0.537f, +0.812f, +0.941f,
		+0.5f, -0.5f,//13
		+0.93f, +0.92f, +0.55f,
		+1.0f, +0.0f,//14
		+0.93f, +0.92f, +0.55f,
		+0.5f, +0.5f,//15
		+0.93f, +0.92f, +0.55f,
		+0.0f, +0.0f,//16
		+0.93f, +0.92f, +0.55f,
		+1.0f, +0.0f,//17
		+0.93f, +0.82f, +0.93f,
		+1.0f, +1.0f,//18
		+0.93f, +0.82f, +0.93f,
		+0.5f, +0.5f,//19
		+0.93f, +0.82f, +0.93f,
		+0.0f, -1.0f,//20
		+1.0f, +0.0f, +0.0f,
		+1.0f, -1.0f,//21
		+1.0f, +0.0f, +0.0f,
		+1.0f, +0.0f,//22
		+1.0f, +0.0f, +0.0f,
	};
	sizeofVerts = sizeof(verts);

	GLuint myBufferID;
	GLushort indices[] = { 0,1,2, 3,4,5, 6,7,8, 9,11,12, 9,10,11, 13,14,16, 14,15,16, 17,18,19, 20,21,22};
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);
	/*glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLushort indices[] = { 0,1,2, 0,3,4 };
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts) + sizeof(indices), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(verts), sizeof(indices), indices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));
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
	glDrawElements(GL_TRIANGLES, 27, GL_UNSIGNED_SHORT, (void*)sizeofVerts);

}