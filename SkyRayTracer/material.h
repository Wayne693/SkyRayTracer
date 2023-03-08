#pragma once
#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include "Texture.h"
#include "onb.h"
#include "pdf.h"

struct scatter_record
{
	Ray speculer_ray;
	bool is_specular;
	color attenuation;
	std::shared_ptr<pdf> pdf_ptr;
};

class Material
{
public:
	virtual bool scatter(const Ray& rin, const HitRecord& rec, scatter_record& srec) const
	{
		return false;
	}

	virtual color scattering_pdf(const Ray& rin, const HitRecord& rec, const Ray& scattered) const
	{
		return vec3(0, 0, 0);
	}

	virtual color emitted(const Ray& rin, const HitRecord& rec, float u, float v, const point3& p) const {
		return color(0, 0, 0);
	}
};

//class Lambertian :public Material
//{
//public:
//	Lambertian(const color& a) : albedo(std::make_shared<Solid>(a)) {}
//	Lambertian(std::shared_ptr<Texture> a) :albedo(a) {}
//
//	virtual bool scatter(const Ray& rin, const HitRecord& rec, scatter_record& srec) const override
//	{
//		onb uvw;
//		uvw.build_from_w(rec.normal);
//		auto direction = uvw.local(random_cosine_direction());
//		srec.speculer_ray = Ray(rec.p, direction);
//		srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
//		srec.is_specular = false;
//		srec.pdf_ptr = std::make_shared<CosPDF>(rec.normal);
//		return true;
//	}
//
//	float scattering_pdf(const Ray& rin, const HitRecord& rec, const Ray& scattered) const
//	{
//		return 0.5 / PI;
//		/*auto cosine = dot(rec.normal, scattered.direction().normalized());
//		return cosine < 0 ? 0 : cosine / PI;*/
//	}
//
//private:
//	std::shared_ptr<Texture> albedo;
//};
//
//class Metal :public Material
//{
//public:
//	Metal(const color& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}
//
//	virtual bool scatter(const Ray& rin, const HitRecord& rec, scatter_record& srec) const override
//	{
//		auto reflected = Reflect(rin.direction().normalized(), rec.normal);
//		srec.speculer_ray = Ray(rec.p, reflected + fuzz * RandomInUnitSphere());
//		srec.attenuation = albedo;
//		srec.is_specular = true;
//		srec.pdf_ptr = 0;
//		return true;
//	}
//
//private:
//	color albedo;
//	float fuzz;
//};
//
//class Dielectric :public Material
//{
//public:
//	Dielectric(float refractionIndex) :ir(refractionIndex) {}
//
//	virtual bool scatter(const Ray& rin, const HitRecord& rec, scatter_record& srec) const override
//	{
//		srec.attenuation = color(1, 1, 1);
//		float refractionRatio = rec.front_face ? (1.f / ir) : ir;
//
//		vec3 dir = rin.direction().normalized();
//		float cost = fmin(dot(-dir, rec.normal), 1.0);
//		float sint = sqrt(1.f - cost * cost);
//
//		bool cantRefract = refractionRatio * sint > 1.f;
//		if (cantRefract || reflectance(cost,refractionRatio) > Random())
//			dir = Reflect(dir, rec.normal);
//		else dir = Refract(dir, rec.normal, refractionRatio);
//
//
//		srec.speculer_ray = Ray(rec.p, dir);
//		srec.is_specular = false;
//		srec.pdf_ptr = 0;
//		return true;
//	}
//private:
//	float ir;
//
//private:
//	static double reflectance(double cosine, double ref_idx) {
//		// Use Schlick's approximation for reflectance.
//		auto r0 = (1 - ref_idx) / (1 + ref_idx);
//		r0 = r0 * r0;
//		return r0 + (1 - r0) * pow((1 - cosine), 5);
//	}
//};

class DiffuseLight :public Material
{
public:
	DiffuseLight(std::shared_ptr<Texture> a) : emit(a){}
	DiffuseLight(color col) :emit(std::make_shared<Solid>(col)) {}

	virtual bool scatter(const Ray& rin, const HitRecord& rec, scatter_record& srec) const override
	{
		return false;
	}

	virtual color emitted(const Ray& rin, const HitRecord& rec, float u, float v, const point3& p) const override
	{
		return emit->value(u, v, p);
	}

public:
	std::shared_ptr<Texture> emit;
};

float NDFGGX(vec3 n, vec3 h, float r)
{
	float sqrta = r * r;
	float ndoth = fmax(0.f, dot(n, h));
	float sqrtndoth = ndoth * ndoth;

	float nom = sqrta;
	float denom = (sqrtndoth * (sqrta - 1.f) + 1.f);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(vec3 n, vec3 v, float k)
{
	float nom = dot(n, v);
	float denom = nom * (1.f - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 n, vec3 v, vec3 l, float k)
{
	float sggx1 = GeometrySchlickGGX(n, v, k);
	float sggx2 = GeometrySchlickGGX(n, l, k);

	return sggx1 * sggx2;
}

vec3 FresnelSchlick(vec3 n, vec3 v, vec3 f0)
{
	auto ndotv = dot(n, v);
	return f0 + (vec3(1, 1, 1) - f0) * pow(1.f - ndotv, 5);
}


class CookTorrance :public Material
{
public:

	CookTorrance(color a, float r):albedo(a), roughness(r)
	{
		f0 = vec3(0.04, 0.04, 0.04);
	}
	CookTorrance(color a, float r, vec3 f):albedo(a),roughness(r),f0(f) {}
	

	virtual bool scatter(const Ray& rin, const HitRecord& rec, scatter_record& srec) const
	{
		onb uvw;
		uvw.build_from_w(rec.normal);
		auto direction = uvw.local(random_cosine_direction());
		srec.speculer_ray = Ray(rec.p, direction);
		srec.attenuation = albedo;
		srec.is_specular = false;
		srec.pdf_ptr = std::make_shared<CosPDF>(rec.normal);
		return true;
	}

	vec3  scattering_pdf(const Ray& rin, const HitRecord& rec, const Ray& scattered) const
	{
		vec3 v = -rin.direction().normalized();
		vec3 n = rec.normal.normalized();
		vec3 l = scattered.direction().normalized();
		vec3 h = (v + l).normalized();

		float k = (roughness + 1) * (roughness + 1) / 8;
		vec3 F = FresnelSchlick(n, v, f0);
		color diffuse = (vec3(1, 1, 1) - F) * albedo / PI;

		color specular;
		auto D = NDFGGX(n, h, roughness);
		auto G = GeometrySmith(n, v, l, k);

		auto vdotn = dot(v, n);
		auto ldotn = dot(l, n);

		return diffuse + F * D * G / (4 * vdotn * ldotn);
	}
public:
	color albedo;		//反照率
	float roughness;	//粗糙度
	vec3 f0;			//平面反射率
};