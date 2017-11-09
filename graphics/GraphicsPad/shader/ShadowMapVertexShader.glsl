#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 uv;
in layout(location=4) vec4 tangent;

uniform mat4 fullTransformMatrix ;

out vec2 textureCoordinate;

void main()
{
	gl_Position = fullTransformMatrix * vec4(position, 1.0);
	textureCoordinate = uv;
}