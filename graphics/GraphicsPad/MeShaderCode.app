const char* vertexShaderCode = 
	"#version 430\r\n"
	""
	"in layout(location = 0) vec2 position;"
	"in layout(location = 1) vec3 vertexColor;"
	""
	"out vec3 theColor;"
	""
	"void main()"
	"{"
	"	gl_Position = vec4(position, 0.0, 1.0);"
	"	theColor = (0.0f, 1.0f, 0.0f);"
	"}";

const char* fragmentShaderCode =
	"#version 430\r\n"
	""
	"out vec4 daColor;"
	"in vec3 theColor;"
	""
	""
	"void main()"
	"{"
	"	daColor = vec4(theColor, 1.0);"
	"}";

