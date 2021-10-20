#version 460 core

uniform vec4 aColor;

out vec4 oColor;

void main()
{
	vec3 rgb = aColor.rgb;
	rgb /= (rgb + vec3(1.0));
	rgb = pow(rgb, vec3(1.0 / 2.2));
	oColor = vec4(rgb, aColor.a);
}
