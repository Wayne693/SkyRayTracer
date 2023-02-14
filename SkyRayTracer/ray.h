#pragma once
#include "vec3.h"

class Ray
{
public:
	Ray() {}
	Ray(const point3& origin, const vec3& direction) :orig(origin), dir(direction) {}

	point3 origin() const { return orig; }
	point3 direction() const { return dir; }

	//P(t) = A + tb;
	point3 at(double t) const {
		return orig + t * dir;
	}

private:
	point3 orig;
	vec3 dir;
};