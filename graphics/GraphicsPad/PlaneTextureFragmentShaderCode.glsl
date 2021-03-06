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
	//vec4 texColor = texture(myTexture, textureCoordinate);
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);
		
	// Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float specularIntensity = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1); //Gloss
	vec4 specularLight = vec4(specularIntensity * vec3(1.0, 0.0, 0.0), 1.0);
 
	// Attenuation
	float distanceToLight = length(lightPositionWorld - vertexPositionWorld);
	float attenuation = 1.0 / (1.0 + 0.1 * pow(distanceToLight, 2));

	daColor = ambientLight + attenuation * (diffuseLight + specularLight);
}