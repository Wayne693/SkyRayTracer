#pragma once
#include "vec3.h"
#include "iostream"
#include "math.h"

void Draw(std::ostream& out, color pixelColor, int sampleTimes)
{
	pixelColor /= sampleTimes;
	auto r = sqrt(pixelColor.x());
	auto g = sqrt(pixelColor.y());
	auto b = sqrt(pixelColor.z());
	out << static_cast<int>(256 * Clamp(r, 0, 0.999)) << ' ' << static_cast<int>(256 * Clamp(g, 0, 0.999)) << ' ' << static_cast<int>(256 * Clamp(b, 0, 0.999)) << '\n';
}