#version 430

in vec3 texcoord;

uniform samplerCube cubemap;

out vec4 daColor;

void main()
{
	daColor = texture(cubemap, texcoord);
}