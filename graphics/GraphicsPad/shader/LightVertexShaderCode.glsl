#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;

uniform mat4 lightTransformMatrix;

out vec3 theColor;

void main()
{
	vec4 v = vec4(position,1.0);
	gl_Position = lightTransformMatrix * v;
	theColor = vertexColor;
}