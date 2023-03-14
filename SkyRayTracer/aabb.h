#pragma once
#include "vec3.h"
#include "ray.h"

class Aabb
{
public:
	__device__ Aabb() {}
	__device__ Aabb(const point3& a, const point3& b)
	{
		minp = a;
		maxp = b;
	}

	__device__ point3 min() const { return minp; }
	__device__ point3 max() const { return maxp; }

	__device__ bool hit(const Ray& r, float mint, float maxt) const
	{
		for (int i = 0; i < 3; i++)
		{
			auto t0 = fmin((minp[i] - r.origin()[i]) / r.direction()[i], ((maxp[i] - r.origin()[i]) / r.direction()[i]));
			auto t1 = fmin((minp[i] - r.origin()[i]) / r.direction()[i], ((maxp[i] - r.origin()[i]) / r.direction()[i]));
			mint = fmax(mint, t0);
			maxt = fmin(maxt, t1);
			if (maxt <= mint) return false;
		}
		return true;
	}
private:
	point3 minp;
	point3 maxp;
};

__device__ inline Aabb SurroundingBox(Aabb b0, Aabb b1)
{
	vec3 small(fmin(b0.min().x(), b1.min().x()), fmin(b0.min().y(), b1.min().y()), fmin(b0.min().x(), b1.min().y()));
	vec3 big(fmax(b0.max().x(), b1.max().x()), fmax(b0.max().y(), b1.max().y()), fmax(b0.max().x(), b1.max().y()));

	return Aabb(small, big);
}