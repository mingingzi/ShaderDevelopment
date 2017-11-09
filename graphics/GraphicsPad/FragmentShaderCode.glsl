#version 430

in vec3 vertexPositionWorld;
in vec2 textureCoordinate;
in mat4 tangentToModelTransform;
in vec3 normalModelVertex;

uniform vec3 lightPositionWorld;
uniform vec4 ambientLight;
uniform vec3 eyePositionWorld;
uniform sampler2D myTexture;
uniform sampler2D normalMap;
uniform samplerCube cubemap;
uniform mat4 modelToWorldMatrix ;

out vec4 daColor;

void main()
{	
	// Check whether face is lit
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	vec3 worldSpaceVertexNormal = vec3(modelToWorldMatrix * vec4(normalModelVertex, 0.0));
	float checkLit = ceil(dot(lightVectorWorld, normalize(worldSpaceVertexNormal)));

	// normalmap
	vec4 normalmapColor = texture(normalMap,textureCoordinate);
	vec3 tangentSpaceNormal = (normalmapColor.xyz - vec3(0.5f, 0.5f, 0.5f))*2;
	vec3 modelSpaceNormal = vec3(tangentToModelTransform * vec4(tangentSpaceNormal,0.0));
	vec3 normalWorld = vec3(modelToWorldMatrix * vec4(modelSpaceNormal,0));

	// Diffuse
	float brightness = clamp(dot(lightVectorWorld, normalize(normalWorld)), 0, 1) * checkLit;
	vec4 texColor = texture(myTexture, textureCoordinate);
	//vec4 texColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 diffuseLight = brightness * texColor;

	// Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld) * checkLit;
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float specularIntensity = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1); //Gloss
	vec4 specularLight = vec4(specularIntensity * vec3(0.5, 0.0, 0.0), 1.0);

	// CubeMap Reflection
	vec3 reflectDir = reflect(-eyeVectorWorld, normalWorld);
	vec4 cubeMapColor = texture(cubemap, vec3(reflectDir.x, -reflectDir.yz));
 
	// Attenuation
	float distanceToLight = length(lightPositionWorld - vertexPositionWorld);
	float attenuation = 1.0 / (1.0 + 0.1 * pow(distanceToLight, 2));

	vec4 materialColor = ambientLight + attenuation * (diffuseLight + specularLight);
	daColor = mix(materialColor, cubeMapColor, 0.5);
}