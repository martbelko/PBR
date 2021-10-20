#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uProjection;
uniform mat4 uView;

out vec3 vTexCoords;

void main()
{
	vTexCoords = aPos;
	vec4 pos = uProjection * uView * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}
