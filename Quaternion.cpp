#include "Quaternion.h"
#include "3DMath.h"
#include <cmath>

namespace Math3D {
	 Quaternion::Quaternion(float _i, float _j, float _k, float _r)
	 	: i(_i), j(_j), k(_k), r(_r) {}

		Quaternion::Quaternion(const Vec3f& axis, float angle) {
			float half_angle = angle * 0.5f;
			float s = sin(half_angle);
			i = axis[0] * s;
			j = axis[1] * s;
			k = axis[2] * s;
			r = cos(half_angle);
		}

	Quaternion::Quaternion(const Xformf& rot) {
		float trace = rot.sum_trace();
		if (trace > 0.0f) {
			float s = sqrt(trace + 1.0f) * 2.0f;
			r = 0.25f * s;
			i = (rot[2][1] - rot[1][2]) / s;
			j = (rot[0][2] - rot[2][0]) / s;
			k = (rot[1][0] - rot[0][1]) / s;
		} else if ((rot[0][0] > rot[1][1]) && (rot[0][0] > rot[2][2])) {
			float s = sqrt(1.0f + rot[0][0] - rot[1][1] - rot[2][2]) * 2.0f;
			r = (rot[2][1] - rot[1][2]) / s;
			i = 0.25f * s;
			j = (rot[0][1] + rot[1][0]) / s;
			k = (rot[0][2] + rot[2][0]) / s;
		} else if (rot[1][1] > rot[2][2]) {
			float s = sqrt(1.0f + rot[1][1] - rot[0][0] - rot[2][2]) * 2.0f;
			r = (rot[0][2] - rot[2][0]) / s;
			i = (rot[0][1] + rot[1][0]) / s;
			j = 0.25f * s;
			k = (rot[1][2] + rot[2][1]) / s;
		} else {
			float s = sqrt(1.0f + rot[2][2] - rot[0][0] - rot[1][1]) * 2.0f;
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

	float Quaternion::Mag() const {
		return sqrt(Dot(*this));
	}

	Quaternion operator/(float f, const Quaternion& q) {
		return q.operator/(f);
	}

	Quaternion lerp(const Quaternion& a, const Quaternion& b, float t) {
		return a * (1.0f - t) + b * t;
	}

	// TODO optimization pass
	Quaternion slerp(const Quaternion& a, const Quaternion& b, float t) {
		// Compute the cosine of the angle between the two quaternions
		float cos_theta = a.Dot(b);
		Quaternion b_copy = b;
		if (cos_theta < 0.0f) {
			b_copy = b * -1.0f;
			cos_theta = -cos_theta;
		}

		constexpr const float THRESHOLD = 0.9995f;
		if (cos_theta > THRESHOLD) {
			return lerp(a, b_copy, t).Normalize();
		} else {
			float angle = acos(cos_theta);
			float sin_angle = sqrt(1.0f - cos_theta * cos_theta);
			float w1 = sin((1.0f - t) * angle) / sin_angle;
			float w2 = sin(t * angle) / sin_angle;
			return (a * w1 + b_copy * w2).Normalize();
		}
	}

	// ZYX Sequence
	Vec3f Quaternion::Euler() const {
		return Vec3f {
			atan2(2.0f * (r * i + j * k), 1.0f - 2.0f * (i * i + j * j)),
			asin(2.0f * (r * j - k * i)),
			atan2(2.0f * (r * k + i * j), 1.0f - 2.0f * (j * j + k * k))
		};
	}

	bool nearly_equal(const Quaternion& a, const Quaternion& b) {
		return nearly_equal(a.i, b.i) 
			&& nearly_equal(a.j, b.j) 
			&& nearly_equal(a.k, b.k) 
			&& nearly_equal(a.r, b.r);
	}
}