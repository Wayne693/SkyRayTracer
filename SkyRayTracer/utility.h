#pragma once
#include <cmath>
#include <limits>
#include <cstdlib>
#include "cuda_runtime.h"
#include <iostream>
#include <curand_kernel.h>

#define __hd__ __host__ __device__

#pragma region Constants

extern float INF;
extern float PI;

#pragma endregion


#pragma region Utility Function

__hd__ inline float DegreesToRadians(float degrees)
{
	return degrees * PI / 180.f;
}

__device__ inline float Random(curandState& rs)
{
	return curand_uniform(&rs);
}

__device__ inline float Random(float min, float max, curandState& rs)
{
	return min + (max - min) * Random(rs);
}

__device__ inline float Randomi(int min, int max, curandState& rs)
{
	return static_cast<int>(Random(min, max + 1, rs));
}

//random real in [0,1)
//__host__ inline float Random()
//{
//	return rand() / (RAND_MAX + 1.f);
//}
//
//__host__ inline double Random(float min, float max)
//{
//	return min + (max - min) * Random();
//}
//
//__host__ inline int Randomi(int min, int max) {
//	// Returns a random integer in [min,max].
//	return static_cast<int>(Random(min, max + 1));
//}

__hd__ inline float Clamp(float x, float min, float max)
{
	return fmin(max, fmax(x, min));
}
#pragma endregion

#pragma region CUDA_Utility

#define CheckCudaError(val) CheckCuda( (val), #val, __FILE__, __LINE__ )
inline void CheckCuda(cudaError_t result, char const* const func, const char* const file, int const line)
{
	if (result)
	{
		std::cerr << "CUDA error = " << static_cast<unsigned int>(result) << " at " << file << ":" << line << " '" << func << "' \n";
		cudaDeviceReset();
		exit(99);
	}
}
#pragma endregion
