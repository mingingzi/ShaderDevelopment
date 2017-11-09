#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 uv;

uniform mat4 skyboxTransformMatrix;

out vec3 texcoord;

void main()
{
	vec4 v = vec4(position,1.0);
	gl_Position = skyboxTransformMatrix * v;
	texcoord = vec3(position.x, -position.yz);
}