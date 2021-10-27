#version 460 core

layout(location = 0) in vec2 aPos; // from [-1,-1] to [1,1]

uniform mat4 uInvProjView;
uniform float uNear;
uniform float uFar;

out vec3 vOrigin;
out vec3 vRay;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	vOrigin = (uInvProjView * vec4(aPos, -1.0, 1.0) * uNear).xyz;
	vRay = (uInvProjView * vec4(aPos * (uFar - uNear), uFar + uNear, uFar - uNear)).xyz;
}
