#pragma once
#include <cmath>
#include <limits>
#include <cstdlib>

#pragma region Constants

const float INF = std::numeric_limits<float>::infinity();
const double PI = acos(-1);

#pragma endregion


#pragma region Utility Function

inline float DegreesToRadians(float degrees)
{
	return degrees * PI / 180.f;
}

//random real in [0,1)
inline float Random()
{
	return rand() / (RAND_MAX + 1.f);
}

inline double Random(float min, float max)
{
	return min + (max - min) * Random();
}

inline float Clamp(float x, float min, float max)
{
	return std::min(max, std::max(x, min));
}
#pragma endregion
