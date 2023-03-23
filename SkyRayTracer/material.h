#pragma once
#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include "Texture.h"
#include "onb.h"
#include "pdf.h"
#include "thrust/extrema.h"

struct scatter_record
{
	Ray speculer_ray;
	bool is_specular;
	bool is_dielectric;
	color attenuation;
	pdf* pdf_ptr;
	onb curonb;
};

class Material
{
public:
	__device__ virtual bool scatter(Ray& rin, const HitRecord& rec, scatter_record& srec) const
	{
		return false;
	}

	__device__ virtual color scattering_pdf(Ray& rin, const HitRecord& rec, const Ray& scattered) const
	{
		return vec3(0, 0, 0);
	}

	__device__ virtual color emitted(Ray& rin, const HitRecord& rec, float u, float v, const point3& p) const 
	{
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

class Dielectric :public Material
{
public:
	__device__ Dielectric(float refractionIndex) :ir(refractionIndex) {}

	__device__ virtual bool scatter(Ray& rin, const HitRecord& rec, scatter_record& srec) const override
	{
 		srec.attenuation = color(1.f, 1.f, 1.f);
		float refractionRatio = rec.front_face ? (1.f / ir) : ir;

		//if (!rec.front_face)
		//{
		//	/*printf("HahHahHa\n");*/
		//	debug(rin.direction());
		//}
		//printf("ff = %d\n", rec.front_face);
		vec3 dir = rin.direction().normalized();
		float cost = thrust::min(dot(-dir, rec.normal), 1.f);
		float sint = sqrt(1.f - cost * cost);

		bool cantRefract = refractionRatio * sint > 1.f;
		if (cantRefract || reflectance(cost,refractionRatio) > Random(rin.randstate()))
			dir = Reflect(dir, rec.normal);
		else 
		dir = Refract(dir, rec.normal, refractionRatio);


		srec.speculer_ray = Ray(rec.p, dir, rin.randstate());
		srec.is_specular = false;
		srec.is_dielectric = true;
		srec.pdf_ptr = 0;
		return true;
	}
private:
	float ir;

private:
	__device__ static double reflectance(double cosine, double ref_idx) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};

class DiffuseLight :public Material
{
public:
	__device__ DiffuseLight(Texture* a) : emit(a){}
	__device__ DiffuseLight(color col)
	{
		emit = new Solid(col);
	}
	
	__device__ ~DiffuseLight()
	{
		delete emit;
	}

	__device__ virtual bool scatter( Ray& rin, const HitRecord& rec, scatter_record& srec) const override
	{
		return false;
	}

	__device__ virtual color emitted( Ray& rin, const HitRecord& rec, float u, float v, const point3& p) const override
	{
		return emit->value(u, v, p);
	}

public:
	Texture* emit;
};

__device__ inline float NDFGGX(vec3 n, vec3 h, float r)
{
	float sqrta = r * r;
	float ndoth = fmax(0.f, dot(n, h));
	float sqrtndoth = ndoth * ndoth;

	float nom = sqrta;
	float denom = (sqrtndoth * (sqrta - 1.f) + 1.f);
	denom = PI * denom * denom;

	return nom / denom;
}

__device__ inline float GeometrySchlickGGX(vec3 n, vec3 v, float k)
{
	float nom = dot(n, v);
	float denom = nom * (1.f - k) + k;

	return nom / denom;
}

__device__ inline float GeometrySmith(vec3 n, vec3 v, vec3 l, float k)
{
	float sggx1 = GeometrySchlickGGX(n, v, k);
	float sggx2 = GeometrySchlickGGX(n, l, k);

	return sggx1 * sggx2;
}

__device__ inline vec3 FresnelSchlick(vec3 n, vec3 v, vec3 f0)
{
	auto ndotv = dot(n, v);
	return f0 + (vec3(1, 1, 1) - f0) * pow(1.f - ndotv, 5);
}


class BRDF :public Material
{
public:
	__device__ BRDF(color a, float r):albedo(a), roughness(r)
	{
		f0 = vec3(0.04, 0.04, 0.04);
		pdf_ptr = new CosPDF();
	}

	__device__ BRDF(color a, float r, vec3 f):albedo(a),roughness(r),f0(f)
	{
		pdf_ptr = new CosPDF();
	}

	__device__ virtual bool scatter(Ray& rin, const HitRecord& rec, scatter_record& srec) const
	{
		onb uvw;
		uvw.build_from_w(rec.normal);
		//auto direction = uvw.local(random_cosine_direction(rin.randstate()));
		//srec.speculer_ray = Ray(rec.p, direction, rin.randstate());
		srec.attenuation = albedo;
		srec.is_specular = false;
		//pdf_ptr->buildonb(rec.normal);//基于法线构造正交基
		srec.pdf_ptr = pdf_ptr;
		srec.curonb = uvw;
		srec.is_dielectric = false;
		//srec.pdf_ptr->buildonb(rec.normal);
		return true;
	}

	__device__ vec3  scattering_pdf(Ray& rin, const HitRecord& rec, const Ray& scattered) const
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
	pdf* pdf_ptr;		//采样pdf
};

//法线、入射光线、入射电介质密度，出射电介质密度
__device__ inline float Fresnel(vec3 n, vec3 wi, float miui, float miu0)
{
	auto c = fabs(dot(wi, n));
	float sqrg = (miu0 * miu0 / miui / miui) - 1 + c * c;

	if (sqrg < 0)//如果g为虚数，则F=1，全反射
	{
		return 1;
	}

	auto g = std::sqrt(sqrg);

	auto gpc = g + c;
	auto gmc = g - c;
	auto cgm1 = c * gpc - 1;
	auto cmm1 = c * gmc - 1;

	return (gmc * gmc / (2 * gpc * gpc)) * (1 + (cgm1 * cgm1 / cmm1 / cmm1));
}

__device__ inline float GGXNDF(float r, vec3 h, vec3 n)
{
	auto nom = r * r * (dot(h, n) > 0 ? 1 : 0);
	auto costh = dot(h.normalized(), n.normalized());
	auto cos2th = costh * costh;
	auto cos4th = cos2th * cos2th;
	auto p = r * r + (1 - cos2th) / cos2th;
	auto p2 = p * p;

	auto denom = PI * cos4th * p2;
	//debug(dot(h, n));
	return nom / denom;
}

__device__ inline float GGXG1(float r, vec3 h, vec3 w, vec3 n)
{
	auto vdotm = dot(w, h);
	auto vdotn = dot(w, n);

	auto costh = dot(w.normalized(), n.normalized());
	auto cos2th = costh * costh;
	auto tanth = (1 - cos2th) / cos2th;

	auto nom = (vdotm / vdotn) > 0 ? 2 : 0;
	auto p = 1 + r * r * tanth * tanth;
	auto denom = 1 + sqrt(p);

	return nom / denom;

}

__device__ inline float GGXGeo(float r, vec3 h, vec3 wo, vec3 wi, vec3 n)
{
	return GGXG1(r, h, wo, n) * GGXG1(r, h, wi, n);
}


class BTDF :public Material
{
public:
	__device__ BTDF(color a, float r) :albedo(a), roughness(r)
	{
		//f0 = vec3(0.04, 0.04, 0.04);
		miu = 1.5f;
		pdf_ptr = new SpherePDF;
	}

	__device__ BTDF(color a, float r, float m) :albedo(a), roughness(r), miu(m)
	{
		//f0 = vec3(0.04, 0.04, 0.04);
		pdf_ptr = new SpherePDF;
	}

	__device__ virtual bool scatter(Ray& rin, const HitRecord& rec, scatter_record& srec) const
	{
		onb uvw;
		uvw.build_from_w(rec.normal);

		//auto direction = uvw.local(RandomInUnitSphere(rin.randstate()));
	/*	if (dot(direction, rec.normal) < 0)
		{
			debug(direction);
		}*/
		//srec.speculer_ray = Ray(rec.p, direction, rin.randstate());
		srec.attenuation = albedo;
		srec.is_specular = false;
		//pdf_ptr->buildonb(rec.normal);//基于法线构造正交基
		//srec.pdf_ptr = pdf_ptr;
		//srec.pdf_ptr->buildonb(rec.normal);
		srec.pdf_ptr = pdf_ptr;
		srec.curonb = uvw;
		return true;
	}

	__device__ vec3  scattering_pdf(Ray& rin, const HitRecord& rec, const Ray& scattered) const
	{
		vec3 wo = -rin.direction();
		vec3 n = rec.normal;
		vec3 wi = scattered.direction();

		float miuo = rec.front_face ? 1.f : miu;
		float miui = (miuo == 1.f) ? miu : 1.f;

		//if (!rec.front_face && rin.direction().x() == rin.direction().x())
		//{
		//	debug(rin.direction());
		//}

		printf("miui = %lf miuo = %lf %d\n", miui, miuo, rec.front_face);
		vec3 ht = -(miuo * wo + miui * wi);//(v + l).normalized();

		float k = (roughness + 1) * (roughness + 1) / 8;
		float F = Fresnel(n, wi, miui, miuo);
		float D = GGXNDF(roughness, ht, n);
		float G = GGXGeo(roughness, ht, wo, wi, n);
		
		auto wodotn = dot(wo, n);
		auto widotn = dot(wi, n);
		auto widotht = dot(wi, ht);
		auto wodotht = dot(wo, ht);
		
		auto nom = miuo * miuo * (vec3(1, 1, 1) - vec3(F, F, F)) * G * D;
		auto tmp = miui * widotht + miuo * wodotht;
		auto denom = tmp * tmp;
		

		auto rt = /*albedo/PI +*/ (fabs(widotht) * fabs(wodotht) / fabs(widotn) / fabs(wodotn)) * (nom / denom);
		//debug(rt);
		
		/*if (miuo == 0.15f)
		{
			debug(miuo);
		}*/
		//debug(G);
		return rt;
		/*color diffuse = (vec3(1, 1, 1) - F) * albedo / PI;

		color specular;
		auto D = NDFGGX(n, ht, roughness);
		auto G = GeometrySmith(n, wo, wi, k);

		*/
	}
public:
	color albedo;		//反照率
	float roughness;	//粗糙度
	//vec3 f0;			//平面反射率
	pdf* pdf_ptr;		//采样pdf
	float miu;			//电介质密度
};