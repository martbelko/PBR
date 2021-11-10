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
	Ray ray;
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

	for (int i = 0; i < 4; ++i)
	{
		spheres[i].radius = bufferSpheres[i].properties.x;
		spheres[i].transparency = bufferSpheres[i].properties.y;
		spheres[i].reflection = bufferSpheres[i].properties.z;
		spheres[i].center = bufferSpheres[i].center.xyz;
		spheres[i].surfaceColor = bufferSpheres[i].surfaceColor.rgb;
	}

	return spheres;
}

Sphere[4] spheres;

const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightPower = 40.0;
const float screenGamma = 2.2;

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
	intersection.ray = ray;

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

// Blinn-Phong + Shadows
vec3 GetFragColorFromIntersection(Intersection intersection)
{
	// Blinn-Phong + Shadows
	vec3 color = spheres[intersection.sphereIndex].surfaceColor;
	vec3 ambient = 0.05 * color;
	vec3 diffuse = vec3(0);
	vec3 specular = vec3(0);

	vec3 lightDir = uLightPosition - intersection.hitPoint;
	float distance = length(lightDir);
	distance = distance * distance;
	lightDir = normalize(lightDir); // Optimize?

	vec3 normal = intersection.normal;
	float dotProd = dot(lightDir, normal);
	float diff = max(dotProd, 0.0);
	if (diff > 0.0)
	{
		diffuse = color * diff * lightColor * lightPower / distance;

		vec3 viewDir = normalize(intersection.ray.origin - intersection.hitPoint);
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = 0.0;
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
		specular = color * spec * lightColor * lightPower / distance;
	}

	// Check for shadow ray
	Ray ray;
	ray.origin = intersection.hitPoint;
	ray.dir = lightDir;
	Intersection lightIntersection = FindNearestIntersection(ray);
	if (intersection.sphereIndex != lightIntersection.sphereIndex && lightIntersection.sphereIndex >= 0)
	{
		diffuse = max(dot(-lightDir, normal), 0.0) * color;
		specular = vec3(0);
	}

	return ambient + diffuse + specular;
}

void main()
{
	spheres = ParseBufferSpheres();
	Ray ray;
	ray.origin = vOrigin;
	ray.dir = normalize(vRay);

	Intersection intersection = FindNearestIntersection(ray);
	vec3 color;
	if (intersection.sphereIndex >= 0) // Hit sphere object
		color = GetFragColorFromIntersection(intersection);
	else if (intersection.sphereIndex == -1) // Hit light
		color = vec3(0.3);
	else // Hit nothing
		color = vec3(0.01, 0.01, 0.01);

	oFragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
