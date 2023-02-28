#pragma once
#include "hittable.h"
#include "memory"
#include "vector"

class HittableList
{
public:
	HittableList() {};
	HittableList(std::shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

	bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
	bool boundingbox(float t0, float t1, Aabb& outputBox) const;
public:
	std::vector<std::shared_ptr<Hittable>> objects;
};

bool HittableList::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	HitRecord tmprec;
	bool hit_anything = false;
	auto closest = tmax;

	for (const auto& object : objects)
	{
		if (object->hit(r, tmin, closest, tmprec))
		{
			hit_anything = true;
			closest = tmprec.t;
			rec = tmprec;
		}
	}

	return hit_anything;
}

bool HittableList::boundingbox(float t0, float t1, Aabb& outputBox) const
{
	if (objects.empty())
	{
		return false;
	}

	Aabb tmpBox;
	bool firstBox = true;

	for (const auto& object : objects)
	{
		if (!object->boundingbox(t0, t1, tmpBox)) return false;
		outputBox = firstBox ? tmpBox : SurroundingBox(outputBox, tmpBox);
		firstBox = false;
	}

	return true;
}