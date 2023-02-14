#pragma once
#include <cmath>
#include <limits>

#pragma region Constants

const float INF = std::numeric_limits<float>::infinity();
const double PI = acos(-1);

#pragma endregion


#pragma region Utility Function

inline float DegreesToRadians(float degrees)
{
	return degrees * PI / 180.f;
}

#pragma endregion
