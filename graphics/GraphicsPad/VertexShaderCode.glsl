#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 uv;

uniform mat4 fullTransformMatrix ;
uniform mat4 modelToWorldMatrix ;

out vec3 normalWorld;
out vec3 vertexPositionWorld;
out vec2 textureCoordinate;

void main()
{
	gl_Position = fullTransformMatrix * vec4(position, 1.0);
	normalWorld = vec3(modelToWorldMatrix * vec4(normalModel, 0));
	vertexPositionWorld = vec3(modelToWorldMatrix * vec4(position, 1));
	textureCoordinate = uv;
}