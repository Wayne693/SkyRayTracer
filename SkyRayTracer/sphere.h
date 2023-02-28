#pragma once
#include "hittable.h"
#include "vec3.h"

class Sphere :public Hittable
{
public:
	Sphere() {}
	Sphere(point3 c, float r, std::shared_ptr<Material> m) :center(c), radius(r), mat_ptr(m) {};

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec)const override;
	virtual bool boundingbox(float t0, float t1, Aabb& outputBox)const override;
private:
	point3 center;
	float radius;
	std::shared_ptr<Material> mat_ptr;
private:
	static void GetSphereUV(const point3& p, float& u, float& v)
	{
		auto theta = acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + PI;

		u = phi / (2 * PI);
		v = theta / PI;
	}
};

bool Sphere::hit(const Ray& r, float tmin, float tmax, HitRecord& rec)const
{
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
	return true;
}

bool Sphere::boundingbox(float t0, float t1, Aabb& outputBox) const
{
	outputBox = Aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}