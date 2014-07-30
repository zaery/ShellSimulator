#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
out vec3 Normal;
out vec3 Position;
out vec2 TexCoord;
uniform mat4 mvp;
uniform mat4 modelView;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
void main()
{
	TexCoord = VertexTexCoord;
	Normal = normalize(NormalMatrix * VertexNormal);
	Position = vec3(modelView * vec4(position, 1.0));
	gl_Position = mvp * vec4(position, 1.0);
}