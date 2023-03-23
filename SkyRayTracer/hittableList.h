#pragma once
#include "hittable.h"
#include "memory"
#include "vector"
#include "sphere.h"



class HittableList
{
public:
	__device__ HittableList() {};
	__device__ HittableList(Hittable** os, int num)
	{
		length = num;
		objects = os;
	}

	__device__ bool hit( Ray& r, float tmin, float tmax, HitRecord& rec) const
	{
		HitRecord tmprec;
		tmprec.threadId = rec.threadId;
		bool hit_anything = false;
		auto closest = tmax;
		
		for (int i = 0; i < length; i++)
		{
			//printf("--list** ray origin = %lf %lf %lf threadId = %d\n", r.origin().x(), r.origin().y(), r.origin().z(), rec.threadId);
			//Test(r, tmprec);
			//if (objects[i]->hit(r, tmin, closest, tmprec))
			//Sphere* sphere = static_cast<Sphere*> (objects[i]);
			//sphere->hit(r, tmin, closest, tmprec, nullptr);

			if(objects[i]->hit(r, tmin, closest, tmprec))
			{
				//printf("*list* hit = %d tmprec_address = %p tmpff = %d tmpmatptr = %p  rec.p = (%lf %lf %lf) tmpthreadId = %d\n", hit_anything, &tmprec, tmprec.front_face, tmprec.mat_ptr, tmprec.p.x(), tmprec.p.y(), tmprec.p.z(), tmprec.threadId);
				//printf("**list** ray origin = %lf %lf %lf threadId = %d\n", r.origin().x(), r.origin().y(), r.origin().z(), rec.threadId);

				hit_anything = true;
				closest = tmprec.t;
				rec = tmprec;
			}
		}
		//printf("*list* hit = %d rec_address = %p ff = %d tmpff = %d matptr = %p threadId = %d\n", hit_anything, &rec, rec.front_face, tmprec.front_face, rec.mat_ptr, rec.threadId);
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
