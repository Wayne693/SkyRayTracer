#pragma once
#include "aarect.h"
#include "hittable.h"
#include "hittableList.h"

class Box : public Hittable
{
public:
	Box() {}
	Box(const point3& p0, const point3& p1, std::shared_ptr<Material> ptr);

	virtual bool hit(const Ray& r, float mint, float maxt, HitRecord& rec) const override;
	virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const override
	{
		outputBox = Aabb(boxMin, boxMax);
		return true;
	}

public:
	vec3 boxMin;
	vec3 boxMax;
	HittableList sides;
};

Box::Box(const point3& p0, const point3& p1, std::shared_ptr<Material> ptr)
{
	boxMin = p0;
	boxMax = p1;

	sides.add(std::make_shared<RectXY>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
	sides.add(std::make_shared<RectXY>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

	sides.add(std::make_shared<RectXZ>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
	sides.add(std::make_shared<RectXZ>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

	sides.add(std::make_shared<RectYZ>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
	sides.add(std::make_shared<RectYZ>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

bool Box::hit(const Ray& r, float mint, float maxt, HitRecord& rec) const
{
	return sides.hit(r, mint, maxt, rec);
}