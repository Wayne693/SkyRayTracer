#pragma once
#include "ray.h"
#include "aabb.h"

class Material;

struct HitRecord
{
	point3 p;
	vec3 normal;
	std::shared_ptr<Material> mat_ptr;
	float t;
	bool front_face;
	float u;
	float v;

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
	virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const = 0;
	virtual float pdf_value(const point3& o, const vec3& v) const 
	{
		return 0;
	}
	virtual vec3 random(const vec3& o) const
	{
		return vec3(1, 0, 0);
	}
};

class Translate : public Hittable
{
public:
	Translate(std::shared_ptr<Hittable> p, const vec3& displacement): ptr(p), offset(displacement) {}

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
	{
		Ray movedr(r.origin() - offset, r.direction());
		if (!ptr->hit(movedr, tmin, tmax, rec))
		{
			return false;
		}

		rec.p += offset;
		rec.set_face_normal(movedr, rec.normal);
		return true;
	}

	virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const
	{
		if (!ptr->boundingbox(t0, t1, outputBox))
		{
			return false;
		}

		outputBox = Aabb(outputBox.min() + offset, outputBox.max() + offset);
		return true;
	}

public:
	std::shared_ptr<Hittable> ptr;
	vec3 offset;
};

class RotateY : public Hittable {
public:
	RotateY(std::shared_ptr<Hittable> p, float angle);

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const override;

	virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const override {
		outputBox = bbox;
		return hasbox;
	}

public:
	std::shared_ptr<Hittable> ptr;
	double sin_theta;
	double cos_theta;
	bool hasbox;
	Aabb bbox;
};

RotateY::RotateY(std::shared_ptr<Hittable> p, float angle) : ptr(p) {
	auto radians = DegreesToRadians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->boundingbox(0, 1, bbox);

	point3 min(INF, INF, INF);
	point3 max(-INF, -INF, -INF);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				auto newx = cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				vec3 tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = Aabb(min, max);
}

bool RotateY::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const {
	auto origin = r.origin();
	auto direction = r.direction();

	origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
	origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

	direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
	direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

	Ray rotated_r(origin, direction);

	if (!ptr->hit(rotated_r, t_min, t_max, rec))
		return false;

	auto p = rec.p;
	auto normal = rec.normal;

	p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
	p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

	normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
	normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

	rec.p = p;
	rec.set_face_normal(rotated_r, normal);

	return true;
}