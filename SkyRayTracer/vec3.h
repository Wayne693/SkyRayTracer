#pragma once
#include <cmath>
#include <iostream>

class vec3
{
public:
	vec3() :v{ 0, 0, 0 } {};
	vec3(float v0, float v1, float v2) : v{ v0,v1,v2 } {}

	float x() const { return v[0]; }
	float y() const { return v[1]; }
	float z() const { return v[2]; }

	vec3 operator-() const { return vec3(-v[0], -v[1], -v[2]); }
	float operator[](int i) const { return v[i]; }
	float& operator[](int i) { return v[i]; }

	vec3& operator += (const vec3& vec)
	{
		v[0] += vec.v[0];
		v[1] += vec.v[1];
		v[2] += vec.v[2];
		return *this;
	}

	vec3& operator *= (const float t)
	{
		v[0] *= t;
		v[1] *= t;
		v[2] *= t;
		return *this;
	}

	vec3& operator /= (const float t)
	{
		return *this *= 1 / t;
	}
	
	//vec3 Utility
	friend std::ostream& operator<<(std::ostream& out, const vec3& vec);
	friend vec3 operator+(const vec3& va, const vec3& vb);
	friend vec3 operator-(const vec3& va, const vec3& vb);
	friend vec3 operator*(const vec3& va, const vec3& vb);
	friend vec3 operator*(const vec3& va, const float t);
	friend vec3 operator*(const float t, const vec3& vb);
	friend vec3 operator/(const vec3& va, const float t);
	friend float dot(const vec3& va, const vec3& vb);
	friend vec3 cross(const vec3& va, const vec3& vb);


	float sqrMagnitude() const {
		return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	}

	float magnitude() const {
		return std::sqrt(sqrMagnitude());
	}

	vec3 normalize() {
		return *this /= magnitude();
	}

	vec3 normalized() {
		return *this / magnitude();
	}
private:
	float v[3];

public:

};

using point3 = vec3;
using color	 = vec3;

#pragma region vec3 Utility

inline std::ostream& operator<<(std::ostream& out, const vec3& vec)
{
	return out << vec.v[0] << ' ' << vec.v[1] << ' ' << vec.v[2];
}

inline vec3 operator+(const vec3& va, const vec3& vb)
{
	return vec3(va.v[0] + vb.v[0], va.v[1] + vb.v[1], va.v[2] + vb.v[2]);
}

inline vec3 operator-(const vec3& va, const vec3& vb)
{
	return vec3(va.v[0] - vb.v[0], va.v[1] - vb.v[1], va.v[2] - vb.v[2]);
}

inline vec3 operator*(const vec3& va, const vec3& vb)
{
	return vec3(va.v[0] * vb.v[0], va.v[1] * vb.v[1], va.v[2] * vb.v[2]);
}

inline vec3 operator*(const vec3& va, const float t)
{
	return vec3(va.v[0] * t, va.v[1] * t, va.v[2] * t);
}

inline vec3 operator*(const float t, const vec3& vb)
{
	return vb * t;
}

inline vec3 operator/(const vec3& va, const float t)
{
	return va * (1 / t);
}

inline float dot(const vec3& va, const vec3& vb)
{
	return va.v[0] * vb.v[0] + va.v[1] * vb.v[1] + va.v[2] * vb.v[2];
}

inline vec3 cross(const vec3& va, const vec3& vb)
{
	return vec3(va.v[1] * vb.v[2] - va.v[2] * vb.v[1], va.v[2] * vb.v[0] - va.v[0] * vb.v[2], va.v[0] * vb.v[1] - va.v[1] * vb.v[0]);
}

#pragma endregion

