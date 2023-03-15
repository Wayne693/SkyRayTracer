#pragma once
#include "vec3.h"
#include "utility.h"
#include "onb.h"
#include "Camera.h"

__device__ inline vec3 random_cosine_direction(curandState& rs) {
	auto r1 = Random(rs);
	auto r2 = Random(rs);
	//printf("%lf %lf\n", r1, r2);
	auto z = sqrt(1 - r2);

	auto phi = 2 * PI * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);
	//debug(vec3(x, y, z));
	return vec3(x, y, z);
}

class pdf
{
public:
	__device__ virtual void buildonb(const vec3& n)
	{
		return;
	}
	__device__ virtual float value(const vec3& direction) const = 0;
	__device__ virtual vec3 generate(curandState& rs) const = 0;
};

class CosPDF : public pdf {
public:
	__device__ CosPDF(const vec3& w) { uvw.build_from_w(w); }
	__device__ CosPDF(){}

	__device__ virtual void buildonb(const vec3& n) override
	{
		uvw.build_from_w(n);
	}

	__device__ virtual float value(const vec3& direction) const override
	{
		auto cosine = dot(direction.normalized(), uvw.w());
		return (cosine <= 0) ? 0 : cosine / PI;
	}

	__device__ virtual vec3 generate(curandState& rs) const override
	{
		vec3 dir = uvw.local(random_cosine_direction(rs));
		//debug(dir);
		return dir;
	}

public:
	onb uvw;
};

class HittablePDF : public pdf
{
public:
	__device__ HittablePDF(Hittable* p, const point3& origin) :ptr(p), o(origin) {}

	__device__ virtual float value(const vec3& direction) const override
	{
		return ptr->pdf_value(o, direction);
	}

	__device__ virtual vec3 generate(curandState& rs) const override
	{
		return ptr->random(o, rs);
	}

public:
	point3 o;
	Hittable* ptr;
};

class MixPDF : public pdf
{
public:
	__device__ MixPDF(pdf* p0, pdf* p1) {
		p[0] = p0;
		p[1] = p1;
	}

	__device__ virtual float value(const vec3& direction) const override 
	{
		return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
	}

	__device__ virtual vec3 generate(curandState& rs) const override
	{
		if (Random(rs) < 0.5)
			return p[0]->generate(rs);
		else
			return p[1]->generate(rs);
	}

public:
	pdf* p[2];
};