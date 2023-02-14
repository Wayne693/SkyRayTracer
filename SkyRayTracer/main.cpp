#include <iostream>
#include <fstream>
#include <string>
#include "vec3.h"
#include "draw.h"
#include "ray.h"
#include "hittableList.h"
#include "sphere.h"
#include "math.h"
#include <memory>

color RayColor(const Ray& r, const HittableList& scene)
{
	HitRecord rec;
	if (scene.hit(r, 0, INF, rec))
	{
		//std::cout << rec.normal << std::endl;
		return 0.5 * (rec.normal + color(1, 1, 1));
	}
	vec3 dir = r.direction().normalized();
	auto t = 0.5 * (dir.y() + 1.f);
	return (1.f - t) * color(1.f, 1.f, 1.f) + t * color(0.5, 0.7, 1.0);
}

int main()
{
	std::string filename = "C:\\Users\\Kingsoft\\Desktop\\image.ppm";
	std::ofstream image(filename.data());

	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	//Scene
	HittableList scene;
	scene.add(std::make_shared<Sphere>(point3(0, 0, -1), 0.5));
	scene.add(std::make_shared<Sphere>(point3(0, -100.5, -1), 100));

	// Camera
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

	//Render
	image << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			auto u = float(i) / (image_width - 1);
			auto v = float(j) / (image_height - 1);

			Ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
			color pixelcolor = RayColor(r, scene);
			Draw(image, pixelcolor); 
		}
	}
	std::cout << "\nDone.\n";
}