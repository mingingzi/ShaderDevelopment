#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 uv;


uniform mat4 planeM2WTransformMatrix ;
uniform mat4 fullTransformMatrix ;

out vec3 normalWorld;
out vec3 vertexPositionWorld;

void main()
{
	gl_Position = fullTransformMatrix * vec4(position, 1.0);
	normalWorld = vec3(planeM2WTransformMatrix * vec4(normalModel, 0));
	vertexPositionWorld = vec3(planeM2WTransformMatrix * vec4(position, 1));
}