#include <iostream>
#include <fstream>
#include <string>
#include "draw.h"
//#include "ray.h"
//#include "sphere.h"
//#include "utility.h"
#include <memory>
#include "material.h"
#include "aarect.h"
#include "aabb.h"
#include "Box.h"
#include "pdf.h"
#include "cuda.cuh"

//color RayColor(const Ray& r,const color& backGround, const HittableList& scene, int depth, Hittable* light)
//{
//	HitRecord rec;
//	float RRp = 0.8f;
//	bool rrflag = depth > 3;
//	if (rrflag && Random() < RRp)
//	{
//		return color(0, 0, 0);
//	}
//	
//
//	if (!scene.hit(r, 0.01, INF, rec))
//	{
//		return backGround;
//	}
//
//	scatter_record srec;
//	color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
//
//	if (!rec.mat_ptr->scatter(r, rec, srec))
//	{
//		return emitted;
//	}
//	
//	if (srec.is_specular)
//	{
//		return srec.attenuation * RayColor(srec.speculer_ray, backGround, scene, depth - 1, light);
//	}
//
//	pdf* light_ptr = &HittablePDF(light, rec.p);
//	MixPDF p(light_ptr, srec.pdf_ptr);
//	auto scattered = Ray(rec.p, p.generate());
//	auto pdfval = p.value(scattered.direction());
//
//	auto ndotwi = dot(rec.normal.normalized(), scattered.direction().normalized());
//	if (!rrflag)
//	{
//		RRp = 0;
//	}
	//return emitted + rec.mat_ptr->scattering_pdf(r, rec, scattered) * RayColor(scattered, backGround, scene, depth + 1, light) * fmax(0, ndotwi) / pdfval / (1 - RRp);
//}



//HittableList random_scene() {
//	HittableList world;
//
//	auto ground_material = std::make_shared<Lambertian>(color(0.5, 0.5, 0.5));
//	world.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));
//
//	for (int a = -11; a < 11; a++) {
//		for (int b = -11; b < 11; b++) {
//			auto choose_mat = Random();
//			point3 center(a + 0.9 * Random(), 0.2, b + 0.9 * Random());
//
//			if ((center - point3(4, 0.2, 0)).magnitude() > 0.9) {
//				std::shared_ptr<Material> sphere_material;
//
//				if (choose_mat < 0.8) {
//					// diffuse
//					auto albedo = color::random() * color::random();
//					sphere_material = std::make_shared<Lambertian>(albedo);
//					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
//				}
//				else if (choose_mat < 0.95) {
//					// metal
//					auto albedo = color::random(0.5, 1);
//					auto fuzz = Random(0, 0.5);
//					sphere_material = std::make_shared<Metal>(albedo, fuzz);
//					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
//				}
//				else {
//					// glass
//					sphere_material = std::make_shared<Dielectric>(1.5);
//					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
//				}
//			}
//		}
//	}
//
//	auto material1 = std::make_shared<Dielectric>(1.5);
//	world.add(std::make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));
//
//	auto material2 = std::make_shared<Lambertian>(color(0.4, 0.2, 0.1));
//	world.add(std::make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));
//
//	auto material3 = std::make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
//	world.add(std::make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));
//
//	return world;
//}

//HittableList simple_light()
//{
//	HittableList objects;
//
//	auto objtex = std::make_shared<Solid>(0.5, 0.7, 1.0);
//	objects.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(objtex)));
//	objects.add(std::make_shared<Sphere>(point3(0, 2, 0), 2, std::make_shared<Lambertian>(objtex)));
//
//	auto diffLight = std::make_shared<DiffuseLight>(color(4, 4, 4));
//	objects.add(std::make_shared<RectXY>(1, 5, 1, 5, -3, diffLight));
//
//	return objects;
//}

//HittableList cornell_box() {
//	HittableList objects;
//
//	auto red = std::make_shared<CookTorrance>(color(.65, .05, .05), 0.5);
//	auto white = std::make_shared<CookTorrance>(color(.73, .73, .73), 0.5);
//	auto green = std::make_shared<CookTorrance>(color(.12, .45, .15), 0.5);
//	auto light = std::make_shared<DiffuseLight>(color(15, 15, 15));
//	auto gold = std::make_shared<CookTorrance>(color(1, 0.71, 0.29), 0.05, vec3(1, 0.71, 0.29));
//	auto sliver = std::make_shared<CookTorrance>(color(0.91, 0.92, 0.92), 0.1, vec3(0.91, 0.92, 0.92));
//
//	objects.add(std::make_shared<RectYZ>(0, 555, 0, 555, 555, green));
//	objects.add(std::make_shared<RectYZ>(0, 555, 0, 555, 0, red));
//	objects.add(std::make_shared<RectXZ>(213, 343, 227, 332, 554, light));
//	objects.add(std::make_shared<RectXZ>(0, 555, 0, 555, 0, white));
//	objects.add(std::make_shared<RectXZ>(0, 555, 0, 555, 555, white));
//	objects.add(std::make_shared<RectXY>(0, 555, 0, 555, 555, white));
//
//	std::shared_ptr<Hittable> box1 = std::make_shared<Box>(point3(0, 0, 0), point3(165, 330, 165), gold);
//	box1 = std::make_shared<RotateY>(box1, 15);
//	box1 = std::make_shared<Translate>(box1, vec3(265, 0, 295));
//	objects.add(box1);
//
//	std::shared_ptr<Hittable> box2 = std::make_shared<Box>(point3(0, 0, 0), point3(165, 165, 165), white);
//	box2 = std::make_shared<RotateY>(box2, -18);
//	box2 = std::make_shared<Translate>(box2, vec3(130, 0, 65));
//	objects.add(box2);
//
//	std::shared_ptr<Hittable> sphere1 = std::make_shared<Sphere>(point3(0, 0, 0), 75, sliver);
//	sphere1 = std::make_shared<Translate>(sphere1, vec3(250, 275, 245));
//	objects.add(sphere1);
//
//	return objects;
//}

int main()
{
	std::string filename = "C:\\Users\\Kingsoft\\Desktop\\image.ppm";
	std::ofstream image(filename.data());

	// Image
	const auto aspect_ratio = 1;
	const int image_width = 600;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int sampleTimes = 10000;
	const int Step = 0;

	//FrameBuffer
	int num_pixels = image_width * image_height;
	size_t fbsize = num_pixels * sizeof(vec3);

	vec3* fb;
	CheckCudaError(cudaMallocManaged((void**)&fb, fbsize));

	//Render
	CudaRender(fb, image_width, image_height, sampleTimes);

	//Write in file
	image << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			auto u = float(i) / (image_width - 1);
			auto v = float(j) / (image_height - 1);
			
			size_t pixel_index = j * image_width + i;
			Draw(image, fb[pixel_index], sampleTimes);
		}
	}
	std::cout << "\nDone.\n";
	return 0;
}