#version 460 core

in vec3 vTexCoords;

uniform samplerCube uSkybox;

out vec4 oFragColor;

void main()
{
	oFragColor = texture(uSkybox, vTexCoords);
}
