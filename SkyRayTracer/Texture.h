#pragma once
#include"vec3.h"

class Texture
{
public: 
	__device__ virtual color value(float u, float v, const point3& p) const = 0;
};

class Solid :public Texture
{
public:
	__device__ Solid() {}
	__device__ Solid(color col) :colorval(col) {}
	__device__ Solid(float r, float g, float b) :colorval(color(r,g,b)) {}

	__device__ virtual color value(float u, float v, const point3& p) const override
	{
		return colorval;
	}

private:
	color colorval;
};