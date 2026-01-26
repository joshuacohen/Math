#pragma once
#include "Matrix.h"
#include <cmath>

namespace Math3D {
	class Quaternion {
	public:
		Quaternion() = default;
		Quaternion(float _i, float _j, float _k, float _r);
		Quaternion(const Vec3f& axis, float angle);
		Quaternion(const Xformf& rot);

		~Quaternion() = default;

		Quaternion operator*(const Quaternion& q) const;
		Quaternion operator*(float f) const;
		Quaternion& operator*= (const Quaternion& q) { *this = *this * q; return *this; }
		Quaternion& operator*= (float f) { *this = *this * f; return *this; }

		Quaternion operator/(const Quaternion& q) const;
		Quaternion operator/(float f) const;
		Quaternion& operator/=(const Quaternion& q);
		Quaternion& operator/=(float f);

		Quaternion operator+(const Quaternion& q) const;
		Quaternion operator+(float f) const;
		Quaternion& operator+=(const Quaternion& q);
		Quaternion& operator+=(float f);

		Quaternion operator-(const Quaternion& q) const;
		Quaternion operator-(float f) const;
		Quaternion& operator-=(const Quaternion& q);
		Quaternion& operator-=(float f);

		Xformf ToRot();
		float Dot(const Quaternion& q) const { return this->vals[0] * q.vals[0] + this->vals[1] * q.vals[1] + this->vals[2] * q.vals[2] + this->vals[3] * q.vals[3]; };
		float Mag() const { return std::sqrt(this->Dot(*this)); }
		Quaternion Normalize() const { return (*this) / Mag(); }
		
	private:
		union {
			float vals[4];
			struct {float i, j, k, r;};
		};
	};

	Quaternion Slerp(const Quaternion& A, const Quaternion& b, float t);
	Quaternion operator/(const Quaternion& q, float f);
	Quaternion operator/(float f, const Quaternion& q);
}