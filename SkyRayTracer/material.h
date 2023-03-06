#pragma once
#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include "Texture.h"
#include "onb.h"

inline vec3 random_cosine_direction() {
	auto r1 = Random();
	auto r2 = Random();
	auto z = sqrt(1 - r2);

	auto phi = 2 * PI * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}

class Material
{
public:
	virtual bool scatter(const Ray& rin, const HitRecord& rec, color& attenuation, Ray& scattered, float& pdf) const
	{
		return false;
	}

	virtual float scattering_pdf(const Ray& rin, const HitRecord& rec, const Ray& scattered) const
	{
		return 0;
	}

	virtual color emitted(float u, float v, const point3& p) const {
		return color(0, 0, 0);
	}
};

class Lambertian :public Material
{
public:
	Lambertian(const color& a) : albedo(std::make_shared<Solid>(a)) {}
	Lambertian(std::shared_ptr<Texture> a) :albedo(a) {}

	virtual bool scatter(const Ray& rin, const HitRecord& rec, color& attenuation, Ray& scattered, float& pdf) const override
	{
		onb uvw;
		uvw.build_from_w(rec.normal);
		auto direction = uvw.local(random_cosine_direction());
		//auto scatterDir = RandomInHemisphere(rec.normal);
		//scattered = Ray(rec.p, scatterDir);
		scattered = Ray(rec.p, direction);
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		//auto cosine = dot(rec.normal, scattered.direction().normalized());
		//pdf = 0.5 / PI;//²ÉÑùpdf
		pdf = dot(uvw.w(), scattered.direction()) / PI;
		//attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}

	float scattering_pdf(const Ray& rin, const HitRecord& rec, const Ray& scattered) const
	{
		auto cosine = dot(rec.normal, scattered.direction().normalized());
		return cosine < 0 ? 0 :cosine / PI;
	}

private:
	std::shared_ptr<Texture> albedo;
};

class Metal :public Material
{
public:
	Metal(const color& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(const Ray& rin, const HitRecord& rec, color& attenuation, Ray& scattered, float& pdf) const override
	{
		auto reflected = Reflect(rin.direction().normalized(), rec.normal);
		scattered = Ray(rec.p, reflected + fuzz * RandomInUnitSphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

private:
	color albedo;
	float fuzz;
};

class Dielectric :public Material
{
public:
	Dielectric(float refractionIndex) :ir(refractionIndex) {}

	virtual bool scatter(const Ray& rin, const HitRecord& rec, color& attenuation, Ray& scattered, float& pdf) const override
	{
		attenuation = color(1, 1, 1);
		float refractionRatio = rec.front_face ? (1.f / ir) : ir;

		vec3 dir = rin.direction().normalized();
		float cost = fmin(dot(-dir, rec.normal), 1.0);
		float sint = sqrt(1.f - cost * cost);

		bool cantRefract = refractionRatio * sint > 1.f;
		if (cantRefract || reflectance(cost,refractionRatio) > Random())
			dir = Reflect(dir, rec.normal);
		else dir = Refract(dir, rec.normal, refractionRatio);


		scattered = Ray(rec.p, dir);
		return true;
	}
private:
	float ir;

private:
	static double reflectance(double cosine, double ref_idx) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};

class DiffuseLight :public Material
{
public:
	DiffuseLight(std::shared_ptr<Texture> a) : emit(a){}
	DiffuseLight(color col) :emit(std::make_shared<Solid>(col)) {}

	virtual bool scatter(const Ray& rin, const HitRecord& rec, color& attenuation, Ray& scattered, float& pdf) const override
	{
		return false;
	}

	virtual color emitted(float u, float v, const point3& p)const override
	{
		return emit->value(u, v, p);
	}

public:
	std::shared_ptr<Texture> emit;
};

