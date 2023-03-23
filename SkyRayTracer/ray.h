#pragma once
#include "vec3.h"
//#include "camera.h"
class Ray
{
public:
	__hd__ Ray() {}
	__hd__ Ray(const point3& origin, const vec3& direction, curandState& randstate) :orig(origin), dir(direction),rs(randstate) {}
	__hd__ Ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

	__hd__ point3 origin() const { return orig; }
	__hd__ point3 direction() const { return dir; }
	__device__ curandState& randstate() { return rs; }

	//P(t) = A + tb;
	__device__  point3 at(double t) const {
		return orig + t * dir;
	}

private:
	point3 orig;
	vec3 dir;
	curandState rs;
};