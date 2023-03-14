#pragma once
#include "hittable.h"
#include "vec3.h"

class RectXY :public Hittable
{
public:
	__device__ RectXY() {};
	__device__ RectXY(float _x0, float _x1, float _y0, float _y1, float _k, Material* mat) :x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat)
	{

	}

	__device__ virtual bool hit(Ray& r, float mint, float maxt, HitRecord& rec)const override
	{
		auto t = (k - r.origin().z()) / r.direction().z();
		if (t < mint || t> maxt)
		{
			return false;
		}

		auto x = r.origin().x() + t * r.direction().x();
		auto y = r.origin().y() + t * r.direction().y();

		if (x<x0 || x>x1 || y<y0 || y>y1)
		{
			return false;
		}
		rec.u = (x - x0) / (x1 - x0);
		rec.v = (y - y0) / (y1 - y0);
		rec.t = t;
		auto outward_normal = vec3(0, 0, 1);
		rec.set_face_normal(r, outward_normal);
		rec.mat_ptr = mp;
		rec.p = r.at(t);
		return true;
	}

	__device__ virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const override
	{
		outputBox = Aabb(vec3(x0, y0, k - 0.0001), vec3(x1, y1, k + 0.0001));
		return true;
	}

	public:
		Material* mp;
		float x0, x1, y0, y1, k;
};

class RectXZ :public Hittable
{
public:
	__device__ RectXZ() {};
	__device__ RectXZ(float _x0, float _x1, float _z0, float _z1, float _k, Material* mat) :x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat)
	{

	}

	__device__ virtual bool hit(Ray& r, float mint, float maxt, HitRecord& rec)const override
	{
		auto t = (k - r.origin().y()) / r.direction().y();
		if (t < mint || t> maxt)
		{
			return false;
		}

		auto x = r.origin().x() + t * r.direction().x();
		auto z = r.origin().z() + t * r.direction().z();

		if (x<x0 || x>x1 || z<z0 || z>z1)
		{
			return false;
		}
		rec.u = (x - x0) / (x1 - x0);
		rec.v = (z - z0) / (z1 - z0);
		rec.t = t;
		auto outward_normal = vec3(0, 1, 0);
		rec.set_face_normal(r, outward_normal);
		rec.mat_ptr = mp;
		rec.p = r.at(t);
		return true;
	}

	 __device__ virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const override
	{
		outputBox = Aabb(vec3(x0, k - 0.0001, z0), vec3(x1, k + 0.0001, z1));
		return true;
	}
	__device__ virtual float pdf_value(const point3& origin, const vec3& v)const override
	{
		HitRecord rec;
		Ray tmpr(origin, v);
		if (!this->hit(tmpr, 0.001, INF, rec))
			return 0;

		auto area = (x1 - x0) * (z1 - z0);
		auto distance_squared = rec.t * rec.t * v.sqrMagnitude();
		auto cosine = fabs(dot(v, rec.normal) / v.magnitude());

		return distance_squared / (cosine * area);
	}

	__device__ virtual vec3 random(const point3& origin, curandState& rs) const override {
		auto random_point = point3(Random(x0, x1, rs), k, Random(z0, z1, rs));
		return random_point - origin;
	}
public:
	Material* mp;
	float x0, x1, z0, z1, k;
};

class RectYZ :public Hittable
{
public:
	__device__  RectYZ() {};
	__device__ RectYZ(float _y0, float _y1, float _z0, float _z1, float _k, Material* mat) :y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat)
	{

	}

	__device__ virtual bool hit( Ray& r, float mint, float maxt, HitRecord& rec)const override
	{
		auto t = (k - r.origin().x()) / r.direction().x();
		if (t < mint || t> maxt)
		{
			return false;
		}

		auto z = r.origin().z() + t * r.direction().z();
		auto y = r.origin().y() + t * r.direction().y();
		//printf("%lf %lf %lf %lf %lf %lf\n", r.direction().x(), r.direction().y(), r.direction().z(), z, y, k);
		if (z<z0 || z>z1 || y<y0 || y>y1)
		{
			return false;
		}
		rec.u = (y - y0) / (y1 - y0);
		rec.v = (z - z0) / (z1 - z0);
		rec.t = t;
		auto outward_normal = vec3(1, 0, 0);
		rec.set_face_normal(r, outward_normal);
		rec.mat_ptr = mp;
		rec.p = r.at(t);
		
		return true;
	}
	 __device__ virtual bool boundingbox(float t0, float t1, Aabb& outputBox) const override
	{
		outputBox = Aabb(vec3(k - 0.0001, y0, z0), vec3(k + 0.0001, y1, z1));
		return true;
	}

public:
	Material* mp;
	float y0, y1, z0, z1, k;
};


//__device__ bool RectXY::hit(const Ray& r, float mint, float maxt, HitRecord& rec) const


//__device__ bool RectXZ::hit(const Ray& r, float mint, float maxt, HitRecord& rec) const


//__device__ bool RectYZ::hit(const Ray& r, float mint, float maxt, HitRecord& rec) const
