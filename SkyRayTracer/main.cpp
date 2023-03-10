#include <iostream>
#include <fstream>
#include <string>
#include "vec3.h"
#include "draw.h"
#include "ray.h"
#include "hittableList.h"
#include "sphere.h"
#include "math.h"
#include "Camera.h"
#include <memory>
#include "material.h"
#include "aarect.h"
#include "aabb.h"
#include "Box.h"
#include "pdf.h"

color RayColor(const Ray& r,const color& backGround, const HittableList& scene, int depth, std::shared_ptr<Hittable> light)
{
	HitRecord rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (!scene.hit(r, 0.01, INF, rec))
	{
		return backGround;
	}

	scatter_record srec;
	color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, srec))
	{
		return emitted;
	}
	
	if (srec.is_specular)
	{
		return srec.attenuation * RayColor(srec.speculer_ray, backGround, scene, depth - 1, light);
	}

	auto light_ptr = std::make_shared<HittablePDF>(light, rec.p);
	MixPDF p(light_ptr, srec.pdf_ptr);
	auto scattered = Ray(rec.p, p.generate());
	auto pdfval = p.value(scattered.direction());

	auto ndotwi = dot(rec.normal.normalized(), scattered.direction().normalized());
	return emitted + rec.mat_ptr->scattering_pdf(r, rec, scattered) * RayColor(scattered, backGround, scene, depth - 1, light) * fmax(0, ndotwi) / pdfval;
}

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

HittableList cornell_box() {
	HittableList objects;

	auto red = std::make_shared<CookTorrance>(color(.65, .05, .05), 0.5);
	auto white = std::make_shared<CookTorrance>(color(.73, .73, .73), 0.5);
	auto green = std::make_shared<CookTorrance>(color(.12, .45, .15), 0.5);
	auto light = std::make_shared<DiffuseLight>(color(15, 15, 15));
	auto gold = std::make_shared<CookTorrance>(color(1, 0.71, 0.29), 0.2, vec3(1, 0.71, 0.29));
	auto sliver = std::make_shared<CookTorrance>(color(0.91, 0.92, 0.92), 0.1, vec3(0.91, 0.92, 0.92));

	objects.add(std::make_shared<RectYZ>(0, 555, 0, 555, 555, green));
	objects.add(std::make_shared<RectYZ>(0, 555, 0, 555, 0, red));
	objects.add(std::make_shared<RectXZ>(213, 343, 227, 332, 554, light));
	objects.add(std::make_shared<RectXZ>(0, 555, 0, 555, 0, white));
	objects.add(std::make_shared<RectXZ>(0, 555, 0, 555, 555, white));
	objects.add(std::make_shared<RectXY>(0, 555, 0, 555, 555, white));

	std::shared_ptr<Hittable> box1 = std::make_shared<Box>(point3(0, 0, 0), point3(165, 330, 165), gold);
	box1 = std::make_shared<RotateY>(box1, 15);
	box1 = std::make_shared<Translate>(box1, vec3(265, 0, 295));
	objects.add(box1);

	std::shared_ptr<Hittable> box2 = std::make_shared<Box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = std::make_shared<RotateY>(box2, -18);
	box2 = std::make_shared<Translate>(box2, vec3(130, 0, 65));
	objects.add(box2);

	std::shared_ptr<Hittable> sphere1 = std::make_shared<Sphere>(point3(0, 0, 0), 75, sliver);
	sphere1 = std::make_shared<Translate>(sphere1, vec3(250, 275, 245));
	objects.add(sphere1);

	return objects;
}

int main()
{
	std::string filename = "C:\\Users\\Kingsoft\\Desktop\\image.ppm";
	std::ofstream image(filename.data());

	// Image
	const auto aspect_ratio = 1.0;
	const int image_width = 600;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int sampleTimes = 2000;
	const int maxStep = 50;

	//Scene
	HittableList scene = cornell_box();
	color background = color(0, 0, 0);
	vec3 lookfrom = point3(278, 278, -800);
	vec3 lookat = vec3(278, 278, 0);
	float fov = 40.0;
	{
//auto material_ground = std::make_shared<Lambertian>(color(0.8, 0.8, 0.0));
	////auto material_center = std::make_shared<Lambertian>(color(0.1, 0.2, 0.5));
	//auto material_left = std::make_shared<Dielectric>(1.5);
	//auto material_right = std::make_shared<Metal>(color(1, 0.86, 0.57), 1);

	//scene.add(std::make_shared<Sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	///*scene.add(std::make_shared<Sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	//scene.add(std::make_shared<Sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));*/
	////scene.add(std::make_shared<Sphere>(point3(0.1, 0.0, -0.7), 0.2, material_left));
	//scene.add(std::make_shared<Sphere>(point3(0.0, 0.0, -1.5), 0.5, material_right));
	////scene = random_scene();
	//// Camera
	//point3 lookfrom(0, 0, 0);
	//point3 lookat(0, 0, -1);
	//vec3 vup(0, 1, 0);
	//auto dist_to_focus = 1.0;
	//auto aperture = 0.1;
	}
	
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	Camera camera(lookfrom, lookat, vup, fov, aspect_ratio, 0, dist_to_focus);

	auto lightmtl = std::make_shared<DiffuseLight>(color(15, 15, 15));
	auto light = std::make_shared<RectXZ>(213, 343, 227, 332, 554, lightmtl);
	//Render
	image << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			auto u = float(i) / (image_width - 1);
			auto v = float(j) / (image_height - 1);
			color pixelColor(0, 0, 0);

			for (int s = 0; s < sampleTimes; s++)
			{
				auto u = (i + Random()) / (image_width - 1);
				auto v = (j + Random()) / (image_height - 1);
				Ray r = camera.GetRay(u, v);
				pixelColor += RayColor(r, background, scene, maxStep, light);
			}
			Draw(image, pixelColor, sampleTimes);
		}
	}
	std::cout << "\nDone.\n";
}