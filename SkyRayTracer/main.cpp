#include <iostream>
#include <fstream>
#include <string>
#include "draw.h"
#include <memory>
#include "material.h"
#include "aarect.h"
#include "aabb.h"
#include "Box.h"
#include "pdf.h"
#include "cuda.cuh"

int main()
{
	std::string filename = "C:\\Users\\Kingsoft\\Desktop\\image.ppm";
	std::ofstream image(filename.data());

	// Image
	const auto aspect_ratio = 1;
	const int image_width = 600;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int sampleTimes = 128;
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