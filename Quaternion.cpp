#include "Quaternion.h"

namespace Math3D {
	Quaternion Quaternion::operator*(float f) {
		return Quaternion(i * f, j * f, k * f, r * f);
	}

	Quaternion Quaternion::operator *(const Quaternion& q) {
		return Quaternion(
			r * q.i + i * q.r + j * q.k - k * q.j,
			r * q.j - i * q.k + j * q.r + k * q.i,
			r * q.k + i * q.j - j * q.i + k * q.r,
			r * q.r - i * q.i - j * q.j - k * q.k
		);
	}
}