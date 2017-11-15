#version 430

in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 lightPositionWorld;
uniform vec4 ambientLight;
uniform vec3 eyePositionWorld;
uniform sampler2D shadowMap;
uniform mat4 LightFullTransformMatrix;

out vec4 daColor;

void main()
{
	// Shadow
	vec4 LightclipPos = LightFullTransformMatrix * vec4(vertexPositionWorld, 1.0);
	vec2 shadowmap_uv = vec2(LightclipPos.x / LightclipPos.w,LightclipPos.y / LightclipPos.w) ;
	vec2 shadowmap_uv_fixed = shadowmap_uv * 0.5 + 0.5;
	vec4 shadowmapColor = texture(shadowMap, shadowmap_uv_fixed);
	float shadowed;
	if(shadowmap_uv_fixed.x >= 1 || shadowmap_uv_fixed.x <=0 || shadowmap_uv_fixed.y >= 1 || shadowmap_uv_fixed.y <=0)
		shadowed = 1;
	else
		shadowed = clamp((shadowmapColor.x - LightclipPos.z / LightclipPos.w ) * 100 ,0,1);

	// Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = clamp(dot(lightVectorWorld, normalize(normalWorld)), 0, 1);
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);
	vec4 diffuse = diffuseLight * shadowed;

	// Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float specularIntensity = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1); //Gloss
	vec4 specularLight = vec4(specularIntensity * vec3(1.0, 0.0, 0.0), 1.0);
	vec4 specular = clamp(specularLight * shadowed, 0, 1);

	// Attenuation
	float distanceToLight = length(lightPositionWorld - vertexPositionWorld);
	float attenuation = 1.0 / (1.0 + 0.1 * pow(distanceToLight, 2));

	daColor = ambientLight + attenuation * (diffuse + specular);
}