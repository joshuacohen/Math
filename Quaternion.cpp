#include "Quaternion.h"
#include "3DMath.h"

namespace Math3D {
	 Quaternion::Quaternion(float _i, float _j, float _k, float _r)
	 	: i(_i), j(_j), k(_k), r(_r) {}


	bool Quaternion::operator==(const Quaternion& q) const {
		return i == q.i && j == q.j && k == q.k && r == q.r;
	}

	bool Quaternion::nearly_equal(const Quaternion& q) const {
		return Math3D::nearly_equal(i, q.i) && Math3D::nearly_equal(j, q.j) && Math3D::nearly_equal(k, q.k) && Math3D::nearly_equal(r, q.r);
	}

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

	Quaternion Quaternion::operator/(float f) const {
		return Quaternion(i / f, j / f, k / f, r / f);
	}

	Quaternion operator/(const Quaternion& q, float f) {
		return q.operator/(f);
	}

	Quaternion operator/(float f, const Quaternion& q) {
		return q.operator/(f);
	}
}