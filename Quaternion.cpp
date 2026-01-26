#include "Quaternion.h"

namespace Math3D {
	 Quaternion::Quaternion(float _i, float _j, float _k, float _r)
	 	: i(_i), j(_j), k(_k), r(_r) {}

	Quaternion Quaternion::operator*(float f) const {
		return Quaternion(i * f, j * f, k * f, r * f);
	}

	Quaternion Quaternion::operator *(const Quaternion& q) const {
		return Quaternion(
			r * q.i + i * q.r + j * q.k - k * q.j,
			r * q.j - i * q.k + j * q.r + k * q.i,
			r * q.k + i * q.j - j * q.i + k * q.r,
			r * q.r - i * q.i - j * q.j - k * q.k
		);
	}

	Quaternion Quaternion::operator+(const Quaternion& q) const {
		return Quaternion(i + q.i, j + q.j, k + q.k, r + q.r);
	}

	Quaternion Quaternion::operator+(float f) const {
		return Quaternion(i + f, j + f, k + f, r + f);
	}

	Quaternion Quaternion::operator-(const Quaternion& q) const {
		return Quaternion(i - q.i, j - q.j, k - q.k, r - q.r);
	}

	Quaternion Quaternion::operator-(float f) const {
		return Quaternion(i - f, j - f, k - f, r - f);
	}
}