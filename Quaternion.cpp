#include "Quaternion.h"
#include "3DMath.h"

namespace Math3D {
	 Quaternion::Quaternion(float _i, float _j, float _k, float _r)
	 	: i(_i), j(_j), k(_k), r(_r) {}

	Quaternion::Quaternion(const Xformf& rot) {
		// TODO: Optimization pass
		float trace = rot[0][0] + rot[1][1] + rot[2][2];
		if (trace > 0.0f) {
			float s = std::sqrt(trace + 1.0f) * 2.0f;
			r = 0.25f * s;
			i = (rot[2][1] - rot[1][2]) / s;
			j = (rot[0][2] - rot[2][0]) / s;
			k = (rot[1][0] - rot[0][1]) / s;
		} else if ((rot[0][0] > rot[1][1]) && (rot[0][0] > rot[2][2])) {
			float s = std::sqrt(1.0f + rot[0][0] - rot[1][1] - rot[2][2]) * 2.0f;
			r = (rot[2][1] - rot[1][2]) / s;
			i = 0.25f * s;
			j = (rot[0][1] + rot[1][0]) / s;
			k = (rot[0][2] + rot[2][0]) / s;
		} else if (rot[1][1] > rot[2][2]) {
			float s = std::sqrt(1.0f + rot[1][1] - rot[0][0] - rot[2][2]) * 2.0f;
			r = (rot[0][2] - rot[2][0]) / s;
			i = (rot[0][1] + rot[1][0]) / s;
			j = 0.25f * s;
			k = (rot[1][2] + rot[2][1]) / s;
		} else {
			float s = std::sqrt(1.0f + rot[2][2] - rot[0][0] - rot[1][1]) * 2.0f;
			r = (rot[1][0] - rot[0][1]) / s;
			i = (rot[0][2] + rot[2][0]) / s;
			j = (rot[1][2] + rot[2][1]) / s;
			k = 0.25f * s;
		}
	}

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