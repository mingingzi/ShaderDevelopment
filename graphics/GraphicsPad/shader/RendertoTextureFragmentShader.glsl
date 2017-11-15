#version 430

in vec2 textureCoordinate;

out vec4 daColor;

uniform sampler2D myTexture;

void main()
{
	daColor = texture(myTexture, textureCoordinate);
}
