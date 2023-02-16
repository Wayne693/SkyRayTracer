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

color RayColor(const Ray& r, const HittableList& scene, int depth)
{
	HitRecord rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (scene.hit(r, 0.01, INF, rec))
	{
		//point3 target = rec.p + rec.normal + RandomInUnitSphere().normalized();//单位球内一点
		//point3 target = rec.p + rec.normal + RandomInUnitSphere().normalized();//单位球面一点
		//point3 target = rec.p + RandomInHemisphere(rec.normal);//单位半球内一点
		//return 0.5 * RayColor(Ray(rec.p, target - rec.p), scene, depth - 1);
		Ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * RayColor(scattered, scene, depth - 1);
		}
		return color(0, 0, 0);
	}
	vec3 dir = r.direction().normalized();
	auto t = 0.5 * (dir.y() + 1.f);
	return (1.f - t) * color(1.f, 1.f, 1.f) + t * color(0.5, 0.7, 1.0);
}

HittableList random_scene() {
	HittableList world;

	auto ground_material = std::make_shared<Lambertian>(color(0.5, 0.5, 0.5));
	world.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = Random();
			point3 center(a + 0.9 * Random(), 0.2, b + 0.9 * Random());

			if ((center - point3(4, 0.2, 0)).magnitude() > 0.9) {
				std::shared_ptr<Material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = std::make_shared<Lambertian>(albedo);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = Random(0, 0.5);
					sphere_material = std::make_shared<Metal>(albedo, fuzz);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = std::make_shared<Dielectric>(1.5);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = std::make_shared<Dielectric>(1.5);
	world.add(std::make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = std::make_shared<Lambertian>(color(0.4, 0.2, 0.1));
	world.add(std::make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = std::make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(std::make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

int main()
{
	std::string filename = "C:\\Users\\Kingsoft\\Desktop\\image.ppm";
	std::ofstream image(filename.data());

	// Image
	const auto aspect_ratio = 3.0 / 2.0;
	const int image_width = 1200;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int sampleTimes = 500;
	const int maxStep = 50;

	//Scene
	HittableList scene;
	/*auto material_ground = std::make_shared<Lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<Lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = std::make_shared<Dielectric>(1.5);
	auto material_right = std::make_shared<Metal>(color(0.8, 0.6, 0.2), 1);

	scene.add(std::make_shared<Sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	scene.add(std::make_shared<Sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	scene.add(std::make_shared<Sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	scene.add(std::make_shared<Sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
	scene.add(std::make_shared<Sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));*/
	scene = random_scene();
	// Camera
	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	Camera camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

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
				pixelColor += RayColor(r, scene, maxStep);
			}
			Draw(image, pixelColor, sampleTimes);
		}
	}
	std::cout << "\nDone.\n";
}