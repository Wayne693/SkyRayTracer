#pragma once
#include "hittable.h"
#include "memory"
#include "vector"

class HittableList
{
public:
	__device__ HittableList() {};
	__device__ HittableList(Hittable** os, int num)
	{
		length = num;
		objects = os;
	}

	//void clear() { objects.clear(); }
	//void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

	__device__ bool hit( Ray& r, float tmin, float tmax, HitRecord& rec) const
	{
		HitRecord tmprec;
		bool hit_anything = false;
		auto closest = tmax;

		for (int i = 0; i < length; i++)
		{
			if (objects[i]->hit(r, tmin, closest, tmprec))
			{
				hit_anything = true;
				closest = tmprec.t;
				rec = tmprec;
			}
		}

		//for (const auto& object : objects)
		//{
		//	if (object->hit(r, tmin, closest, tmprec))
		//	{
		//		hit_anything = true;
		//		closest = tmprec.t;
		//		rec = tmprec;
		//	}
		//}

		return hit_anything;
	}

	__device__ bool boundingbox(float t0, float t1, Aabb& outputBox) const
	{
		if (length <= 0)
		{
			return false;
		}

		Aabb tmpBox;
		bool firstBox = true;

		for (int i = 0; i < length; i++)
		{
			if (!objects[i]->boundingbox(t0, t1, tmpBox)) return false;
			outputBox = firstBox ? tmpBox : SurroundingBox(outputBox, tmpBox);
			firstBox = false;
		}

		//for (const auto& object : objects)
		//{
		//	if (!object->boundingbox(t0, t1, tmpBox)) return false;
		//	outputBox = firstBox ? tmpBox : SurroundingBox(outputBox, tmpBox);
		//	firstBox = false;
		//}

		return true;
	}
public:
	Hittable** objects;
	int length;
};

//bool HittableList::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const


//bool HittableList::boundingbox(float t0, float t1, Aabb& outputBox) const
