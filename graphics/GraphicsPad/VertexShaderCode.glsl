#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 uv;
in layout(location = 4) vec4 tangent;

uniform mat4 fullTransformMatrix ;
uniform mat4 modelToWorldMatrix ;

out vec3 vertexPositionWorld;
out vec2 textureCoordinate;
out mat4 tangentToModelTransform;

void main()
{
	gl_Position = fullTransformMatrix * vec4(position, 1.0);
	vertexPositionWorld = vec3(modelToWorldMatrix * vec4(position, 1));
	textureCoordinate = uv;
	vec3 bitangentVector = normalize(cross(normalModel,vec3(tangent)) * tangent.w);
 	tangentToModelTransform = mat4(vec4(vec3(tangent),0.0),vec4(bitangentVector,0.0),vec4(normalModel,0.0),vec4(position,1.0));
}