#pragma once
#include "vec3.h"
#include "ray.h"



__device__ void inline debug(vec3 v)
{
	printf("%lf %lf %lf\n", v.x(), v.y(), v.z());
}

__device__ void inline debug(float f)
{
	printf("%lf\n", f);
}

class Camera
{
public:
	__device__ Camera(point3 pos, vec3 lookat, vec3 up, float fov, float aspect, float aperture, float focusDist)
	{
		auto theta = DegreesToRadians(fov);
		auto viewportHeight = 2 * tan(theta / 2);
		auto viewportWidth = viewportHeight * aspect;

		 w = (pos - lookat).normalized();
		 u = cross(up, w).normalized();
		 v = cross(w, u);
		 
		origin = pos;
		hor = focusDist * viewportWidth * u;
		ver = focusDist * viewportHeight * v;
		lowerLeftCorner = origin - focusDist * w - ver / 2 - hor / 2;

		lenRadius = aperture / 2;
	}

	__device__ Ray GetRay(float s, float t, curandState& rs) const
	{
		//vec3 rd = lenRadius * RandomInUnitDisk();
		//vec3 offset = u * rd.x() + v * rd.y();
		vec3 offset = vec3(0, 0, 0);
		vec3 tmp = lowerLeftCorner + s * hor + t * ver - origin - offset;
		//printf("%lf %lf %lf %lf %lf\n", lowerLeftCorner.x(), lowerLeftCorner.y(), lowerLeftCorner.z(), s, t);
		//printf("%lf %lf %lf\n", hor.x(), hor.y(), hor.z());s
		return Ray(origin + offset, tmp, rs);
	}

public:
	point3 origin;
	point3 lowerLeftCorner;
	vec3 hor;
	vec3 ver;
	vec3 u, v, w;
	float lenRadius;
};