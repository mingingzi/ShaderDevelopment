#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

uniform mat3 transform;

out vec3 theColor;

void main()
{
	gl_Position = vec4(vec2(transform * position), 0.0,1.0);
	theColor = vertexColor;
}