#pragma once
#include "vec3.h"
#include "Camera.h"
class onb
{
public:
	__device__ onb(){}

	__device__ inline vec3 operator[](int i) const
	{
		return axis[i];
	}

	__device__ vec3 u() const { return axis[0]; }
	__device__ vec3 v() const { return axis[1]; }
	__device__ vec3 w() const { return axis[2]; }

	__device__ vec3 local(float a, float b, float c) const
	{
		return a * u() + b * v() + c * w();
	}
	//将输入向量转移到local空间
	__device__ vec3 local(const vec3& a) const
	{
		return a.x() * u() + a.y() * v() + a.z() * w();
	}

	__device__ void build_from_w(const vec3& n)
	{
		axis[2] = n.normalized();
		vec3 a = (fabs(w().x()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
		axis[1] = cross(w(), a).normalized();
		axis[0] = cross(w(), v());

		//debug(axis[0]);
	}

public:
	vec3 axis[3];
};

//void onb::build_from_w(const vec3& n)
