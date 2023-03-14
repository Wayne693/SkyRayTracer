#pragma once
#include "aarect.h"
#include "hittable.h"
#include "hittableList.h"

class Box : public Hittable
{
public:
	__device__ Box() {}
	__device__ Box(const point3& p0, const point3& p1, Material* ptr)
	{
		boxMin = p0;
		boxMax = p1;

		Hittable** rects = (Hittable**)malloc(6 * sizeof(Hittable*));
		*(rects + 0) = new RectXY(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
		*(rects + 1) = new RectXY(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr);

		*(rects + 2) = new RectXZ(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
		*(rects + 3) = new RectXZ(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr);

		*(rects + 4) = new RectYZ(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
		*(rects + 5) = new RectYZ(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr);

		sides = HittableList(rects, 6);

		//printf("%lf %lf %lf\n",rects[0], rects)
		//sides.add(std::make_shared<RectXY>);
		//sides.add(std::make_shared<RectXY>);

		//sides.add(std::make_shared<RectXZ>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
		//sides.add(std::make_shared<RectXZ>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr)); 

		//sides.add(std::make_shared<RectYZ>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
		//sides.add(std::make_shared<RectYZ>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
	}

	__device__ virtual bool hit(Ray& r, float mint, float maxt, HitRecord& rec) const override
	{
		//printf("okok\n");
		return sides.hit(r, mint, maxt, rec);
	}
	__device__ virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const override
	{
		outputBox = Aabb(boxMin, boxMax);
		return true;
	}

public:
	vec3 boxMin;
	vec3 boxMax;
	HittableList sides;
};

//__device__ Box::Box(const point3& p0, const point3& p1, Material* ptr)


//bool Box::hit(const Ray& r, float mint, float maxt, HitRecord& rec) const
