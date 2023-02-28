#pragma once
#include"vec3.h"

class Texture
{
public: 
	virtual color value(float u, float v, const point3& p) const = 0;
};

class Solid :public Texture
{
public:
	Solid() {}
	Solid(color col) :colorval(col) {}
	Solid(float r, float g, float b) :colorval(color(r,g,b)) {}

	virtual color value(float u, float v, const point3& p) const override
	{
		return colorval;
	}

private:
	color colorval;
};