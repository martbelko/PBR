#version 460 core

in vec3 vOrigin;
in vec3 vRay;

uniform vec2 uResolution;

out vec4 oFragColor;

float sphere(vec3 ro, vec3 rd, vec3 sc, float r)
{
	vec3 tro = ro - sc;
	float a = dot(rd, rd);
	float b = 2.0 * dot(rd, tro);
	float c = dot(tro, tro) - (r * r);
	float D = (b * b) - (4.0 * a * c);
	if (D < 0.0)
		return -1.0;
	float sqrtD = sqrt(D);
	float nom1 = -b + sqrtD;
	float nom2 = -b - sqrtD;
	float denom = 2.0 * a;

	float r1 = nom1 / denom;
	float r2 = nom2 / denom;
	return min(r1, r2);
}

vec3 background(float t, vec3 rd)
{
	vec3 light = normalize(vec3(sin(t), 0.6, cos(t)));
	float sun = max(0.0, dot(rd, light));
	float sky = max(0.0, dot(rd, vec3(0.0, 1.0, 0.0)));
	float ground = max(0.0, -dot(rd, vec3(0.0, 1.0, 0.0)));
	return
		(pow(sun, 256.0)+0.2*pow(sun, 2.0))*vec3(2.0, 1.6, 1.0) +
		pow(ground, 0.5)*vec3(0.4, 0.3, 0.2) +
		pow(sky, 1.0)*vec3(0.5, 0.6, 0.7);
}

void main()
{
	vec2 uv = (-1.0 + 2.0 * gl_FragCoord.xy / uResolution.xy) *
		vec2(uResolution.x / uResolution.y, 1.0);
	vec3 rd = normalize(vRay);
	vec3 p = vec3(0.0, 0.0, -5.0);
	float t = sphere(vOrigin, rd, p, 1.0);
	vec3 nml = normalize(p - (vOrigin + rd * t));
	vec3 bgCol = background(5.0, rd);
	rd = reflect(rd, nml);
	vec3 col = background(5.0, rd) * vec3(0.9, 0.8, 1.0);
	if (t < 0.0)
		oFragColor = vec4(bgCol, 1.0);
	else
		oFragColor = vec4(col, 1.0);
}
