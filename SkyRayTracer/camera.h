#pragma once
#include "vec3.h"
#include "ray.h"

class Camera
{
public:
	Camera(point3 pos, vec3 lookat, vec3 up, float fov, float aspect, float aperture, float focusDist)
	{
		auto theta = DegreesToRadians(fov);
		auto viewportHeight = 2 * tan(theta / 2);
		auto viewportWidth = viewportHeight * aspect;
		//auto focal = 1.f;

		 w = (pos - lookat).normalized();
		 u = cross(up, w).normalized();
		 v = cross(w, u);

		origin = pos;
		hor = focusDist * viewportWidth * u;
		ver = focusDist * viewportHeight * v;
		lowerLeftCorner = origin - focusDist * w - ver / 2 - hor / 2;

		lenRadius = aperture / 2;
	}

	Ray GetRay(float s, float t) const
	{
		vec3 rd = lenRadius * RandomInUnitDisk();
		vec3 offset = u * rd.x() + v * rd.y();
		return Ray(origin + offset, lowerLeftCorner + s * hor + t * ver - origin - offset);
	}

private:
	point3 origin;
	point3 lowerLeftCorner;
	vec3 hor;
	vec3 ver;
	vec3 u, v, w;
	float lenRadius;
};