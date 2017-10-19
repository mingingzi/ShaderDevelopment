#version 430

in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 lightPositionWorld;
uniform vec4 ambientLight;
uniform vec3 eyePositionWorld;

out vec4 daColor;
void main()
{
	// Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = clamp(dot(lightVectorWorld, normalize(normalWorld)), 0, 1);
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	// Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	vec4 specularLight = vec4(s, s, s, 1.0);
 
	daColor = diffuseLight + ambientLight + specularLight;
}