#version 460 core

in vec3 vOrigin;
in vec3 vRay;

uniform vec2 uResolution;
uniform vec3 uLightPosition;

struct BufferSphere // std430 layout
{
	vec4 properties; // x = radius, y = transparency, z = reflection
	vec4 center;
	vec4 surfaceColor;
	vec4 emissionColor;
};

struct Sphere // Internal struct
{
	float radius;
	float transparency;
	float reflection;
	vec3 center;
	vec3 surfaceColor;
	vec3 emissionColor;
};

layout(std430, binding = 0) buffer Spheres
{
	BufferSphere bufferSpheres[];
};

out vec4 oFragColor;

Sphere[4] ParseBufferSpheres()
{
	Sphere spheres[4];

	spheres[0].radius = bufferSpheres[0].properties.x;
	spheres[0].transparency = bufferSpheres[0].properties.y;
	spheres[0].reflection = bufferSpheres[0].properties.z;
	spheres[0].center = bufferSpheres[0].center.xyz;
	spheres[0].surfaceColor = bufferSpheres[0].surfaceColor.rgb;
	spheres[0].emissionColor = bufferSpheres[0].emissionColor.rgb;

	spheres[1].radius = bufferSpheres[1].properties.x;
	spheres[1].transparency = bufferSpheres[1].properties.y;
	spheres[1].reflection = bufferSpheres[1].properties.z;
	spheres[1].center = bufferSpheres[1].center.xyz;
	spheres[1].surfaceColor = bufferSpheres[1].surfaceColor.rgb;
	spheres[1].emissionColor = bufferSpheres[1].emissionColor.rgb;

	spheres[2].radius = bufferSpheres[2].properties.x;
	spheres[2].transparency = bufferSpheres[2].properties.y;
	spheres[2].reflection = bufferSpheres[2].properties.z;
	spheres[2].center = bufferSpheres[2].center.xyz;
	spheres[2].surfaceColor = bufferSpheres[2].surfaceColor.rgb;
	spheres[2].emissionColor = bufferSpheres[2].emissionColor.rgb;

	spheres[3].radius = bufferSpheres[3].properties.x;
	spheres[3].transparency = bufferSpheres[3].properties.y;
	spheres[3].reflection = bufferSpheres[3].properties.z;
	spheres[3].center = bufferSpheres[3].center.xyz;
	spheres[3].surfaceColor = bufferSpheres[3].surfaceColor.rgb;
	spheres[3].emissionColor = bufferSpheres[3].emissionColor.rgb;

	return spheres;
}

Sphere[4] spheres;

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

struct Intersection
{
	float distance;
	vec3 hitPoint;
	int sphereIndex;
};

const float MAX_DISTANCE = 1000000000.0;

Intersection FindNearestIntersection(vec3 rayOrigin, vec3 rayDir)
{
	Intersection intersection;
	intersection.sphereIndex = -2;
	intersection.distance = MAX_DISTANCE;

	// Check for scene spheres intersections
	for (int i = 0; i < 4; ++i)
	{
		vec3 p = spheres[i].center;
		float t = sphere(rayOrigin, rayDir, p, 1.0); // TODO: Remove constant radius
		if (t > -0.001 && t < intersection.distance)
		{
			intersection.distance = t;
			intersection.hitPoint = rayOrigin + t * rayDir;
			intersection.sphereIndex = i;
		}
	}

	float lightT = sphere(rayOrigin, rayDir, uLightPosition, 0.5);
	if (lightT > -0.001 && lightT < intersection.distance)
	{
		intersection.distance = lightT;
		intersection.hitPoint = rayOrigin + lightT * rayDir;
		intersection.sphereIndex = -1;
	}

	return intersection;
}

vec3 GetFragColorFromIntersection(Intersection intersection)
{
	vec3 fragColor = spheres[intersection.sphereIndex].surfaceColor;
	vec3 shadowRay = normalize(uLightPosition - intersection.hitPoint);
	Intersection nextInt = FindNearestIntersection(intersection.hitPoint, shadowRay);
	if (nextInt.sphereIndex >= 0)
		fragColor *= 0.5;
	return fragColor;
}

const int MAX_RAY_DEPTH = 3;

vec3 RecursiveRayTrace(vec3 ro, vec3 rd)
{
	Intersection intersection = FindNearestIntersection(ro, rd);
	if (intersection.sphereIndex == -2)
	{
		return vec3(0.0, 0.0, 0.0);
	}
	else
	{
		return GetFragColorFromIntersection(intersection);
	}
}

vec3 trace(vec3 rayorig, vec3 raydir, int depth)
{
	Intersection nearIntersection = FindNearestIntersection(rayorig, raydir);
	float tnear = nearIntersection.distance;
	// if there's no intersection return black or background color
	if (nearIntersection.sphereIndex < 0)
		return vec3(0);
	vec3 surfaceColor = vec3(0); // color of the ray/surfaceof the object intersected by the ray
	vec3 phit = nearIntersection.hitPoint; // point of intersection
	vec3 nhit = normalize(phit - spheres[nearIntersection.sphereIndex].center); // normalized normal at the intersection point
	// If the normal and the view direction are not opposite to each other
	// reverse the normal direction. That also means we are inside the sphere so set
	// the inside bool to true. Finally reverse the sign of IdotN which we want
	// positive.
	float bias = 1e-4; // add some bias to the point from which we will be tracing
	bool inside = false;
	if (dot(raydir, nhit) > 0)
	{
		nhit = -nhit;
		inside = true;
	}

	float transparencyF = spheres[nearIntersection.sphereIndex].transparency;
	float reflectionF = spheres[nearIntersection.sphereIndex].reflection;
	if ((transparencyF > 0 || reflectionF > 0) && depth < MAX_RAY_DEPTH)
	{
		float facingratio = dot(-raydir, nhit);
		// change the mix value to tweak the effect
		float fresneleffect = mix(pow(1 - facingratio, 3), 1.0, 0.1);
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		vec3 refldir = normalize(raydir - nhit * 2.0 * dot(raydir, nhit));
		vec3 reflection = trace(phit + nhit * bias, refldir, depth + 1);
		vec3 refraction = vec3(0);
		// if the sphere is also transparent compute refraction ray (transmission)
		if (transparencyF > 0.0)
		{
			float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
			float cosi = dot(-nhit, raydir);
			float k = 1 - eta * eta * (1 - cosi * cosi);
			vec3 refrdir = normalize(raydir * eta + nhit * (eta *  cosi - sqrt(k)));
			refraction = trace(phit - nhit * bias, refrdir, depth + 1);
		}
		// the result is a mix of reflection and refraction (if the sphere is transparent)
		surfaceColor = (reflectionF * fresneleffect + refraction * (1 - fresneleffect) * transparencyF) * spheres[nearIntersection.sphereIndex].surfaceColor;
	}
	else
	{
		// it's a diffuse object, no need to raytrace any further
		for (uint i = 0; i < 4; ++i)
		{
			if (spheres[i].center.x > 0)
			{
				// this is a light
				vec3 transmission = vec3(1);
				vec3 lightDirection = normalize(spheres[i].center - phit);
				for (uint j = 0; j < 4; ++j)
				{
					if (i != j)
					{
						if (sphere(phit + nhit * bias, lightDirection, spheres[j].center, 1.0) >= 0.0)
						{
							transmission = vec3(0);
							break;
						}
					}
				}

				surfaceColor += spheres[nearIntersection.sphereIndex].surfaceColor * transmission * max(0.0, dot(nhit, lightDirection)) * spheres[i].emissionColor;
			}
		}
	}

	return surfaceColor + spheres[nearIntersection.sphereIndex].emissionColor;
}

void main()
{
	spheres = ParseBufferSpheres();
	vec3 ro = vOrigin;
	vec3 rd = normalize(vRay);

	// oFragColor = vec4(trace(ro, rd, 0), 1.0);
	Intersection intersection = FindNearestIntersection(ro, rd);
	if (intersection.sphereIndex >= 0) // Hit sphere object
		oFragColor = vec4(GetFragColorFromIntersection(intersection), 1.0);
	else if (intersection.sphereIndex == -1) // Hit light
		oFragColor = vec4(0.0, 0.0, 1.0, 1.0);
	else // Hit nothing
		oFragColor = vec4(0.1, 0.1, 0.1, 1.0);
}
