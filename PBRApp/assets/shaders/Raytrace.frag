#version 460 core

in vec3 vOrigin;
in vec3 vRay;

uniform vec3 uLightPosition;
uniform samplerCube uCubemap;

const uint KDTREE_MAX_INDICES = 12; // Multiple of 4
struct KDTreeNode // std430 layout
{
	vec4 boxMin; // vec3
	vec4 boxMax; // vec3
	ivec4 childIndices; // ivec2
	int atomIndices[KDTREE_MAX_INDICES];
};

struct BufferSphere // std430 layout
{
	vec4 properties; // x = radius, y = transparency, z = reflection
	vec4 center;
	vec4 surfaceColor;
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

layout(std430, binding = 1) buffer KDTree
{
	KDTreeNode nodes[];
};

out vec4 oFragColor;

const vec3 lightColor = vec3(1.0, 0.0, 1.0);
const float lightPower = 40.0;
const float screenGamma = 2.2;

float HitSphereOutside(Ray ray, vec3 sphereCenter, float radius)
{
	vec3 tro = ray.origin - sphereCenter;
	float a = dot(ray.dir, ray.dir);
	float b = 2.0 * dot(ray.dir, tro);
	float c = dot(tro, tro) - (radius * radius);
	float D = (b * b) - (4.0 * a * c);
	if (D < 0.0)
		return -1.0;
	float sqrtD = sqrt(D);
	float nom1 = -b - sqrtD;
	float nom2 = -b + sqrtD;
	float denom = 2.0 * a;

	float r1 = nom1 / denom;
	float r2 = nom2 / denom;
	return min(r1, r2);
}

float HitSphereInside(Ray ray, vec3 center, float radius)
{
	vec3 tro = ray.origin - center;
	float a = dot(ray.dir, ray.dir);
	float b = 2.0 * dot(ray.dir, tro);
	float c = dot(tro, tro) - (radius * radius);
	float D = (b * b) - (4.0 * a * c);
	if (D < 0.0)
		return -1.0;
	float sqrtD = sqrt(D);
	float nom1 = -b - sqrtD;
	float nom2 = -b + sqrtD;
	float denom = 2.0 * a;

	float r1 = nom1 / denom;
	float r2 = nom2 / denom;

	if ((r1 > 0.0 && r2 < 0.0) || (r1 < 0.0 && r2 > 0.0))
		return max(r1, r2);

	return -1.0;
}

const float MIN_DISTANCE = -0.001;
const float MAX_DISTANCE = 1000000000.0;

uniform int uSpheresCount;
uniform int uKDTreeNodesCount;

float IntersectAABB(Ray ray, vec3 boxMin, vec3 boxMax)
{
	vec3 tMin = (boxMin - ray.origin) / ray.dir;
	vec3 tMax = (boxMax - ray.origin) / ray.dir;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	if (tNear > tFar)
	{
		return -1.0;
	}

	return tNear;
}

Intersection FindNearestIntersection(Ray ray)
{
	Intersection intersection;
	intersection.sphereIndex = -2;
	intersection.distance = MAX_DISTANCE;
	intersection.ray = ray;

	if (IntersectAABB(ray, nodes[0].boxMin.xyz, nodes[0].boxMax.xyz) > 0.0)
	{
		int index = 0;
		int farNodes[500];
		int nIndex = 0;
		while (true)
		{
			int leftIndex = nodes[index].childIndices[0];
			int rightIndex = nodes[index].childIndices[1];
			if (leftIndex >= 0) // We have children
			{
				KDTreeNode leftNode = nodes[leftIndex];
				KDTreeNode rightNode = nodes[rightIndex];

				float leftDist = IntersectAABB(ray, leftNode.boxMin.xyz, leftNode.boxMax.xyz);
				float rightDist = IntersectAABB(ray, rightNode.boxMin.xyz, rightNode.boxMax.xyz);

				int farNodeIndex = -1;
				int finalIndex = -1;
				if (leftDist > 0.0 && leftDist >= rightDist)
				{
					farNodeIndex = rightIndex;
					finalIndex = leftIndex;
				}
				else if (rightDist > 0.0 && rightDist >= leftDist)
				{
					farNodeIndex = leftIndex;
					finalIndex = rightIndex;
				}

				if (finalIndex == -1)
				{
					break;
				}
				else
				{
					farNodes[nIndex] = farNodeIndex;
					index = finalIndex;
					++nIndex;
				}
			}
			else
			{
				KDTreeNode node = nodes[index];
				for (int i = 0; i < KDTREE_MAX_INDICES; ++i)
				{
					int globalIndex = node.atomIndices[i];
					if (globalIndex < 0)
					{
						break;
					}

					vec3 p = bufferSpheres[globalIndex].center.xyz;
					float t = HitSphereOutside(ray, p, bufferSpheres[globalIndex].properties.x);
					if (t > MIN_DISTANCE && t < intersection.distance)
					{
						intersection.distance = t;
						intersection.hitPoint = ray.origin + t * ray.dir;
						intersection.normal = normalize(intersection.hitPoint - bufferSpheres[globalIndex].center.xyz);
						intersection.sphereIndex = globalIndex;
					}
				}

				if (intersection.distance != MAX_DISTANCE)
				{
					break;
				}

				bool end = true;
				for (int i = nIndex; i >= 0; --i)
				{
					--nIndex;
					if (farNodes[i] >= 0)
					{
						index = farNodes[i];
						farNodes[i] = -1;
						end = false;
						break;
					}
				}

				if (end)
				{
					break;
				}
			}
		}

		// Check for scene spheres intersections
		/*for (int i = 0; i < uSpheresCount; ++i)
		{
			vec3 p = bufferSpheres[i].center.xyz;
			float t = HitSphereOutside(ray, p, bufferSpheres[i].properties.x);
			if (t > MIN_DISTANCE && t < intersection.distance)
			{
				intersection.distance = t;
				intersection.hitPoint = ray.origin + t * ray.dir;
				intersection.normal = normalize(intersection.hitPoint - bufferSpheres[i].center.xyz);
				intersection.sphereIndex = i;
			}
		}*/
	}

	// Check for light intersection
	float lightT = HitSphereOutside(ray, uLightPosition, 0.5);
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
	if (intersection.sphereIndex == -1)
		return lightColor;
	else if (intersection.sphereIndex == -2)
		return texture(uCubemap, intersection.ray.dir).rgb;
	else if (intersection.sphereIndex == -3)
		return vec3(0.0);
	else
		return bufferSpheres[intersection.sphereIndex].surfaceColor.rgb;
}

uniform int uMaxDepth = 1;

vec3 trace(Ray primaryRay)
{
	Intersection[2] gIntersections;
	vec3 gColors[1];

	for (int i = 0; i < 2; ++i)
		gIntersections[i].sphereIndex = -3;
	// Generate all intersections
	{
		gIntersections[0] = FindNearestIntersection(primaryRay);
		int pushIntersectionIndex = 1;
		int currIntersectionIndex = 0;
		while (currIntersectionIndex < uMaxDepth)
		{
			Intersection intersection = gIntersections[currIntersectionIndex];
			if (intersection.sphereIndex < 0) // Ray hits nothing or light
			{
				gIntersections[pushIntersectionIndex].sphereIndex = -3; // Actual intersection color is 0 (black), so nothing gets added to the final color
				gIntersections[pushIntersectionIndex + 1].sphereIndex = -3;
			}
			else
			{
				// Reflect
				{
					vec3 reflectDir = reflect(intersection.ray.dir, intersection.normal);
					Ray reflectRay = Ray(intersection.hitPoint, reflectDir);
					Intersection reflectIntersection = FindNearestIntersection(reflectRay);
					gIntersections[pushIntersectionIndex] = reflectIntersection;
				}

				// Refract
				{
					vec3 refractDir = normalize(refract(intersection.ray.dir, intersection.normal, 1.0 / 1.45));
					Ray refractRay = Ray(intersection.hitPoint + 0.001 * refractDir, refractDir);
					int index = intersection.sphereIndex;
					float dist = HitSphereInside(refractRay, bufferSpheres[index].center.xyz, bufferSpheres[index].properties.x);
					vec3 hitPoint = refractRay.origin + dist * refractRay.dir;
					vec3 normal = -normalize(hitPoint - bufferSpheres[index].center.xyz);

					refractRay.dir = normalize(refract(refractRay.dir, normal, 1.45));
					refractRay.origin = hitPoint + 0.001 * refractRay.dir;

					Intersection refractIntersection = FindNearestIntersection(refractRay);
					if ((pushIntersectionIndex + 1) % 2 != 0)
						return vec3(0);
					gIntersections[pushIntersectionIndex + 1] = refractIntersection;
				}
			}

			pushIntersectionIndex += 2;
			++currIntersectionIndex;
		}
	}

	// Combine all intersections
	{
		gColors[0] = GetFragColorFromIntersection(gIntersections[0]);
		for (int currentDepth = 1; currentDepth < uMaxDepth; ++currentDepth)
		{
			vec3 color = vec3(0.0);
			int index = int(pow(2, currentDepth));
			int offset = index - 1;
			for (int i = 0; i < index; i += 2)
			{
				int finalIndex = offset + i;
				// Reflection
				Intersection intersectionReflect = gIntersections[finalIndex];
				if (finalIndex % 2 != 1)
					return vec3(0);
				color += GetFragColorFromIntersection(intersectionReflect) / index / 2.0;
				// Refraction
				Intersection intersectionRefract = gIntersections[finalIndex + 1];
				color += GetFragColorFromIntersection(intersectionRefract) / index / 2.0;
			}

			gColors[currentDepth] = color;
		}

		vec3 finalColor = vec3(0.0);
		for (int i = 0; i < uMaxDepth; ++i)
			finalColor += gColors[i];
		return finalColor;
	}
}

void main()
{
	Ray ray;
	ray.origin = vOrigin;
	ray.dir = normalize(vRay);

	vec3 color = trace(ray);
	oFragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
