#include "cuda.cuh"
#include "utility.h"
#include "aarect.h"
#include "material.h"
#include "Box.h"
#include "Sphere.h"
#include "hittable.h"
#include "Camera.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <curand_kernel.h>
#include "thrust/extrema.h"

__constant__ float INF;
__constant__ float PI;

__device__ color raycolor(Ray& ray, const color& backGround, const HittableList** scene, Hittable* light)
{
	//color finalcolor = backGround;
	color n = vec3(0, 0, 0);
	color m = vec3(1, 1, 1);

	//Ray r = ray;


	for (int i = 0; i < 100; i++)
	{
		HitRecord rec;
		scatter_record srec;
		color curcolor;
		if (!(*scene)->hit(ray, 0.01f, INF, rec))
		{
			return n + m * backGround;
		}

		color emitted = rec.mat_ptr->emitted(ray, rec, rec.u, rec.v, rec.p);
		
		if (!rec.mat_ptr->scatter(ray, rec, srec))
		{
			return n + m * emitted;
		}

		pdf* light_pdf = &HittablePDF(light, rec.p);
		pdf* cos_pdf = &CosPDF(rec.normal);
		MixPDF p(light_pdf, cos_pdf);
		
		auto scattered = Ray(rec.p, p.generate(ray.randstate()), ray.randstate());

		auto pdfval = p.value(scattered.direction());
		auto ndotwi = dot(rec.normal.normalized(), scattered.direction().normalized());
		
		n = n + m * emitted;
		m = m * rec.mat_ptr->scattering_pdf(ray, rec, scattered) * thrust::max(0.f, ndotwi) / pdfval;
		ray = scattered;
	}

	return color(0, 0, 0);
}

__global__ void render(vec3* fb, curandState* cudaRandState, HittableList** scene, Camera** camera, int width, int height, int spp)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x >= width || y >= height)
	{
		return;
	}

	int pixel_index = y * width + x;
	

	//curandState currentrs;
	

	curand_init(2023, pixel_index, 0, &cudaRandState[pixel_index]);
	//printf("%lf %lf %lf\n", (*camera)->origin.x(), (*camera)->origin.y(), (*camera)->origin.z());
	//printf("%d\n", (*scene)->length);
	
	//printf("%lf %lf %lf %lf %lf\n", r.direction().x(), r.direction().y(), r.direction().z(), u, v);
	auto lm = new DiffuseLight(color(15, 15, 15));
	auto light = RectXZ(213, 343, 227, 332, 554, lm);
	
	color sumcolor = vec3(0, 0, 0);
	for (int i = 0; i < spp; i++)
	{
		float offset_x = Random(cudaRandState[pixel_index]);
		float offset_y = Random(cudaRandState[pixel_index]);
		//printf("%d %lf %lf\n", pixel_index, offset_x, offset_y);
		float u = float(x + offset_x) / float(width);
		float v = float(y + offset_y) / float(height);
		Ray r = (*camera)->GetRay(u, v, cudaRandState[pixel_index]);
		//if (pixel_index == 10584)
		//	printf("idx = %d:%d rdir = (%lf, %lf, %lf) %d\n", pixel_index, i, r.direction().x(), r.direction().y(), r.direction().z(), currentrs.d);

		color cc = raycolor(r, vec3(0, 0, 0), scene, &light);
		float max_sample_intensity = 50;
		cc = vec3(thrust::min(max_sample_intensity, cc[0]), thrust::min(max_sample_intensity, cc[1]), thrust::min(max_sample_intensity, cc[2]));
		sumcolor += cc;
	}

	//debug(sumcolor);
	fb[pixel_index] = sumcolor;
}

__device__ void load_cornell_box(Hittable** objects, HittableList** list, Camera** camera)
{
	//objects
	auto green = new CookTorrance(color(.12, .45, .15), 0.5);
	auto red = new CookTorrance(color(.65, .05, .05), 0.5);
	auto white = new CookTorrance(color(.73, .73, .73), 0.5);
	auto light = new DiffuseLight(color(15, 15, 15));
	auto gold = new CookTorrance(color(1, 0.71, 0.29), 0.05, vec3(1, 0.71, 0.29));
	auto sliver = new CookTorrance(color(0.91, 0.92, 0.92), 0.1, vec3(0.91, 0.92, 0.92));
	auto smoothsliver = new CookTorrance(color(0.91, 0.92, 0.92), 0.005, vec3(0.91, 0.92, 0.92));;

	*objects = new RectYZ(0, 555, 0, 555, 555, green);
	*(objects + 1) = new RectYZ(0, 555, 0, 555, 0, red);
	*(objects + 2) = new RectXZ(213, 343, 227, 332, 554, light);
	*(objects + 3) = new RectXZ(0, 555, 0, 555, 0, white);
	*(objects + 4) = new RectXZ(0, 555, 0, 555, 555, white);
	*(objects + 5) = new RectXY(0, 555, 0, 555, 555, white);

	Hittable* box1 = new Box(point3(0, 0, 0), point3(165, 330, 165), smoothsliver);
	box1 = new RotateY(box1, 15);
	box1 = new Translate(box1, vec3(265, 0, 295));
	*(objects + 6) = box1;

	Hittable* box2 = new Box(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = new RotateY(box2, -18);
	box2 = new Translate(box2, vec3(130, 0, 65));
	*(objects + 7) = box2;

	Hittable* sphere1 = new Sphere(point3(0, 0, 0), 75, sliver);
	sphere1 = new Translate(sphere1, vec3(250, 275, 245));
	*(objects + 8) = sphere1;

	//list
	*list = new HittableList(objects, 9);/////////////9

	//Camera
	color background = color(0, 0, 0);
	vec3 lookfrom = point3(278, 278, -800);
	vec3 lookat = vec3(278, 278, 0);
	float fov = 40.0;
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	const auto aspect_ratio = 1.0;
	*camera = new Camera(lookfrom, lookat, vup, fov, aspect_ratio, 0, dist_to_focus);
}

__global__ void load_scene(Hittable** objects, HittableList** list, Camera** camera)
{
	if (blockIdx.x == 0 && threadIdx.x == 0)
	{
		load_cornell_box(objects, list, camera);
	}
}

__global__ void free_world(Hittable** objects, HittableList** list, Camera** camera)
{
	if (blockIdx.x == 0 && threadIdx.x == 0)
	{
		for (int i = 0; i < 9; i++)///////9
		{
			delete *(objects + i);
		}

		delete *list;

		delete *camera;
	}
}

void CudaRender(vec3* fb, int width, int height, int sampleTimes)
{
	Hittable** cudaList;
	HittableList** cudaObjects;
	Camera** cudaCamera;
	curandState* cudaRandState;

	// Load constant
	float inf = std::numeric_limits<float>::infinity();
	cudaMemcpyToSymbol(INF, &inf, sizeof(float));
	float pi = acos(-1);
	cudaMemcpyToSymbol(PI, &pi, sizeof(float));

	// Load scene
	CheckCudaError(cudaMalloc((void**)&cudaList, 9 * sizeof(Hittable*)));//////9*
	CheckCudaError(cudaMalloc((void**)&cudaObjects, sizeof(HittableList*)));
	CheckCudaError(cudaMalloc((void**)&cudaCamera, sizeof(Camera*)));

	load_scene<<<1,1>>>(cudaList, cudaObjects, cudaCamera);
	CheckCudaError(cudaDeviceSynchronize());
	CheckCudaError(cudaGetLastError());

	//Load rand state
	CheckCudaError(cudaMalloc((void**)&cudaRandState, width * height * sizeof(curandState)));
	
	//Rander
	int blockx = 16;
	int blocky = 16;

	dim3 block(blockx, blocky);
	dim3 grid(width / blockx + 1, height / blocky + 1);

	render << <grid, block >> > (fb, cudaRandState, cudaObjects, cudaCamera, width, height, sampleTimes);
	CheckCudaError(cudaDeviceSynchronize());
	CheckCudaError(cudaGetLastError());
	
	//Free
	free_world << <1, 1 >> > (cudaList, cudaObjects, cudaCamera);
	CheckCudaError(cudaDeviceSynchronize());
	CheckCudaError(cudaGetLastError());
}
