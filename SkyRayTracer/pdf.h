#pragma once
#include "vec3.h"
#include "utility.h"
#include "onb.h"
#include "Camera.h"

__device__ inline vec3 random_cosine_direction(curandState& rs) 
{
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

__device__ inline vec3 random_neg_cosine_direction(curandState& rs)
{
	auto r1 = Random(rs);
	auto r2 = Random(rs);
	//printf("%lf %lf\n", r1, r2);
	auto z = sqrt(1 - r2);

	auto phi = 2 * PI * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);
	//debug(vec3(x, y, z));
	return -vec3(x, y, z);
}

class pdf
{
public:
	__device__ virtual float value(const onb& curonb, const vec3& direction) const = 0;
	__device__ virtual vec3 generate(const onb& curonb, curandState& rs) const = 0;
};

class CosPDF : public pdf {
public:
	__device__ CosPDF(){}

	__device__ virtual float value(const onb& curonb, const vec3& direction) const override
	{
		auto cosine = dot(direction.normalized(), curonb.w());
		return (cosine <= 0) ? 0 : cosine / PI;
	}

	__device__ virtual vec3 generate(const onb& curonb, curandState& rs) const override
	{
		vec3 dir = curonb.local(random_cosine_direction(rs));
		return dir;
	}
};

class SpherePDF : public pdf
{
public:
	__device__ SpherePDF() {}

	__device__ virtual float value(const onb& curonb, const vec3& direction) const override
	{
		/*auto cosine = dot(direction.normalized(), curonb.w());
		return (cosine <= 0) ? 0 : cosine / PI;*/
		return 1 / 4.f / PI;
	}

	__device__ virtual vec3 generate(const onb& curonb, curandState& rs) const override
	{
		vec3 dir = random_neg_cosine_direction(rs);
		
		dir = curonb.local(dir);
		/*if (dir.x() != dir.x())
		{*/
			//printf("curonb** %lf %lf %lf\n", curonb.u().x(), curonb.u().y(), curonb.u().z());
		//}
		//vec3 dir = RandomInUnitSphere(rs);
		return dir;
	}
};

class HittablePDF : public pdf
{
public:
	__device__ HittablePDF(Hittable* p, const point3& origin) :ptr(p), o(origin) {}

	__device__ virtual float value(const onb& curonb, const vec3& direction) const override
	{
		return ptr->pdf_value(o, direction);
	}

	__device__ virtual vec3 generate(const onb& curonb, curandState& rs) const override
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
	__device__ MixPDF(pdf* p0, pdf* p1)
	{
		p[0] = p0;
		p[1] = p1;
	}

	__device__ virtual float value(const onb& curonb, const vec3& direction) const override
	{
		return 0.2 * p[0]->value(curonb, direction) + 0.8 * p[1]->value(curonb, direction);
	}

	__device__ virtual vec3 generate(const onb& curonb, curandState& rs) const override
	{
		if (Random(rs) < 0.2)
			return p[0]->generate(curonb, rs);
		else
			return p[1]->generate(curonb, rs);
	}

public:
	pdf* p[2];
};

//class BHMixPDF : public pdf
//{
//public:
//	__device__ BHMixPDF(pdf* p0, pdf* p1) {
//		p[0] = p0;
//		p[1] = p1;
//	}
//
//	__device__ virtual float value(const vec3& direction) const override
//	{
//		auto vp0 = p[0]->value(direction);
//		auto vp1 = p[1]->value(direction);
//		//auto w0 = vp0 / (vp1 + vp0);
//		//return w0 * vp0 + (1 - w0) * vp1;
//		//return /*0.5 * vp0 + 0.5 * */vp1;
//		auto tp = (0.5 * vp0 + 0.5 * vp1);
//		return  tp * tp / (vp1 + vp0)/0.5;
//	}
//
//	__device__ virtual vec3 generate(curandState& rs) const override
//	{
//		if (Random(rs) < 0.5)
//			return p[0]->generate(rs);
//		else
//			return p[1]->generate(rs);
//	}
//
//public:
//	pdf* p[2];
//};