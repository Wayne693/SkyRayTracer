#pragma once
#include "ray.h"

struct HitRecord
{
	point3 p;
	vec3 normal;
	float t;
	bool front_face;

	inline void set_face_normal(const Ray& r, const vec3& outwardNormal)
	{
		front_face = dot(r.direction(), outwardNormal) < 0;
		normal = front_face ? outwardNormal : -outwardNormal;
	}
};

class Hittable
{
public:
	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const = 0;
};