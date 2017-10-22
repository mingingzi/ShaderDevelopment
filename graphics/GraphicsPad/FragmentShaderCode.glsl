#version 430

in vec3 vertexPositionWorld;
in vec2 textureCoordinate;
in mat4 tangentToModelTransform;

uniform vec3 lightPositionWorld;
uniform vec4 ambientLight;
uniform vec3 eyePositionWorld;
uniform sampler2D myTexture;
uniform sampler2D normalMap;
uniform mat4 modelToWorldMatrix ;

out vec4 daColor;

void main()
{
	//normalmap
	vec4 normalmapcolor = texture(normalMap,textureCoordinate);
	vec3 tangentSpaceNormal = (normalmapcolor.xyz - vec3(0.5f,0.5f,0.5f))*2;
	vec3 modelSpaceNormal = vec3(tangentToModelTransform * vec4(tangentSpaceNormal,0.0));
	vec3 normalWorld = vec3(modelToWorldMatrix * vec4(modelSpaceNormal,0));

	// Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = clamp(dot(lightVectorWorld, normalize(normalWorld)), 0, 1);
	vec4 texColor = texture(myTexture, textureCoordinate);
	vec4 diffuseLight = brightness * texColor;

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