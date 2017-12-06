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
uniform sampler2D RenderTex;

out vec4 daColor;

// Approximates the brightness of a RGB value.
float luma( vec3 color ) {
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void main()
{	
	float dx = 1.0 / 1600;
	float dy = 1.0 / 1000;

	float s00 = luma(texture(RenderTex, textureCoordinate + vec2(-dx, dy)).rgb);
	float s10 = luma(texture(RenderTex, textureCoordinate + vec2(-dx, 0.0)).rgb);
	float s20 = luma(texture(RenderTex, textureCoordinate + vec2(-dx, -dy)).rgb);
	float s01 = luma(texture(RenderTex, textureCoordinate + vec2(0.0, dy)).rgb);
	float s21 = luma(texture(RenderTex, textureCoordinate + vec2(0.0, -dy)).rgb);
	float s02 = luma(texture(RenderTex, textureCoordinate + vec2(dx, dy)).rgb);

	float s12 = luma(texture(RenderTex,textureCoordinate + vec2(dx, 0.0)).rgb);
	float s22 = luma(texture(RenderTex,textureCoordinate + vec2(dx, -dy) ).rgb);
	float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
	float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);
	//daColor = vec4 (1.0, 0.0, 0.0, 1.0);

	float dist = sx * sx + sy * sy;
	if( dist > 0.3f )
		daColor = vec4(1.0);
	else
		daColor = vec4(0.0,0.0,0.0,1.0);
}