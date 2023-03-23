#pragma once
#include "hittable.h"
#include "vec3.h"

class Sphere :public Hittable
{
public:
	__device__ Sphere() {}
	__device__ Sphere(point3 c, float r, Material* m) :center(c), radius(r), mat_ptr(m) {};

	__device__ virtual bool hit(Ray& r, float tmin, float tmax, HitRecord& rec)const override
	{
		//printf("--sphere** ray origin = %lf %lf %lf threadId = %d\n", r.origin().x(), r.origin().y(), r.origin().z(), rec.threadId);
		vec3 oc = r.origin() - center;
		auto a = r.direction().sqrMagnitude();
		auto half_b = dot(r.direction(), oc);
		auto c = oc.sqrMagnitude() - radius * radius;

		auto ansdis = half_b * half_b - a * c;
		if (ansdis < 0)
			return false;
		ansdis = std::sqrt(ansdis);
		auto ans = (-half_b - ansdis) / a;
		if (ans<tmin || ans>tmax)
		{
			ans = (-half_b + ansdis) / a;
			if (ans<tmin || ans>tmax)
			{
				return false;
			}
		}

		rec.t = ans;
		rec.p = r.at(ans);
		vec3 outward_normal = (rec.p - center) / radius;

		rec.set_face_normal(r, outward_normal);

		GetSphereUV(outward_normal, rec.u, rec.v);
		rec.mat_ptr = mat_ptr;

		//printf("**sphere** rec_address = %p ff = %d rec.p = (%lf %lf %lf) threadId = %d\n", &rec, rec.front_face, rec.p.x(), rec.p.y(), rec.p.z(), rec.threadId);
		//printf("**sphere** ray origin = %lf %lf %lf\n", r.origin().x(), r.origin().y(), r.origin().z(), rec.threadId);

		return true;
	}

	__device__ virtual bool boundingbox(float t0, float t1, Aabb& outputBox)const override
	{
		outputBox = Aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
		return true;
	}
private:
	point3 center;
	float radius;
	Material* mat_ptr;
private:
	__device__ static void GetSphereUV(const point3& p, float& u, float& v)
	{
		auto theta = acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + PI;

		u = phi / (2 * PI);
		v = theta / PI;
	}
};
