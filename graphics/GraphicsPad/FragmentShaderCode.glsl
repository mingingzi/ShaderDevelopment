#version 430

in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 lightPositionWorld;
uniform vec4 ambientLight;

out vec4 daColor;
void main()
{
	// Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = clamp(dot(lightVectorWorld, normalize(normalWorld)), 0, 1);
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	daColor = diffuseLight + ambientLight;
}