#version 460 core

in vec3 vOrigin;
in vec3 vRay;

uniform vec3 uLightPosition;

struct BufferSphere // std430 layout
{
	vec4 properties; // x = radius, y = transparency, z = reflection
	vec4 center;
	vec4 surfaceColor;
};

struct Sphere // Internal struct
{
	float radius;
	float transparency;
	float reflection;
	vec3 center;
	vec3 surfaceColor;
};

struct Ray
{
	vec3 origin;
	vec3 dir;
};

struct Intersection
{
	float distance;
	vec3 hitPoint;
	vec3 normal;
	int sphereIndex;
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

	spheres[1].radius = bufferSpheres[1].properties.x;
	spheres[1].transparency = bufferSpheres[1].properties.y;
	spheres[1].reflection = bufferSpheres[1].properties.z;
	spheres[1].center = bufferSpheres[1].center.xyz;
	spheres[1].surfaceColor = bufferSpheres[1].surfaceColor.rgb;

	spheres[2].radius = bufferSpheres[2].properties.x;
	spheres[2].transparency = bufferSpheres[2].properties.y;
	spheres[2].reflection = bufferSpheres[2].properties.z;
	spheres[2].center = bufferSpheres[2].center.xyz;
	spheres[2].surfaceColor = bufferSpheres[2].surfaceColor.rgb;

	spheres[3].radius = bufferSpheres[3].properties.x;
	spheres[3].transparency = bufferSpheres[3].properties.y;
	spheres[3].reflection = bufferSpheres[3].properties.z;
	spheres[3].center = bufferSpheres[3].center.xyz;
	spheres[3].surfaceColor = bufferSpheres[3].surfaceColor.rgb;

	return spheres;
}

Sphere[4] spheres;

float HitSphere(Ray ray, vec3 sphereCenter, float radius)
{
	vec3 tro = ray.origin - sphereCenter;
	float a = dot(ray.dir, ray.dir);
	float b = 2.0 * dot(ray.dir, tro);
	float c = dot(tro, tro) - (radius * radius);
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

const float MIN_DISTANCE = -0.001;
const float MAX_DISTANCE = 1000000000.0;

Intersection FindNearestIntersection(Ray ray)
{
	Intersection intersection;
	intersection.sphereIndex = -2;
	intersection.distance = MAX_DISTANCE;

	// Check for scene spheres intersections
	for (int i = 0; i < 4; ++i)
	{
		vec3 p = spheres[i].center;
		float t = HitSphere(ray, p, spheres[i].radius);
		if (t > MIN_DISTANCE && t < intersection.distance)
		{
			intersection.distance = t;
			intersection.hitPoint = ray.origin + t * ray.dir;
			intersection.normal = normalize(intersection.hitPoint - spheres[i].center);
			intersection.sphereIndex = i;
		}
	}

	// Check for light intersection
	float lightT = HitSphere(ray, uLightPosition, 0.5);
	if (lightT > MIN_DISTANCE && lightT < intersection.distance)
	{
		intersection.distance = lightT;
		intersection.hitPoint = ray.origin + lightT * ray.dir;
		intersection.normal = normalize(intersection.hitPoint - uLightPosition);
		intersection.sphereIndex = -1;
	}

	return intersection;
}

vec3 GetFragColorFromIntersection(Intersection intersection)
{
	vec3 fragColor = spheres[intersection.sphereIndex].surfaceColor;
	Ray ray;
	ray.origin = intersection.hitPoint;
	ray.dir = normalize(uLightPosition - intersection.hitPoint);
	Intersection nextInt = FindNearestIntersection(ray);
	if (intersection.sphereIndex != nextInt.sphereIndex && nextInt.sphereIndex >= 0)
		fragColor *= 0.5;
	return fragColor;
}

void main()
{
	spheres = ParseBufferSpheres();
	Ray ray;
	ray.origin = vOrigin;
	ray.dir = normalize(vRay);

	Intersection intersection = FindNearestIntersection(ray);
	if (intersection.sphereIndex >= 0) // Hit sphere object
		oFragColor = vec4(GetFragColorFromIntersection(intersection), 1.0);
	else if (intersection.sphereIndex == -1) // Hit light
		oFragColor = vec4(0.0, 0.0, 1.0, 1.0);
	else // Hit nothing
		oFragColor = vec4(0.1, 0.1, 0.1, 1.0);
}
