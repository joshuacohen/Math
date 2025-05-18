#include "Quaternion.h"

namespace Math3D {
	Quaternion Quaternion::operator*(const Quaternion& q) {
		return Quaternion {
			vals[3] * q.vals[0] + vals[0] * q.vals[3] + vals[1] * q.vals[2] - vals[2] * q.vals[1],
			vals[3] * q.vals[1] + vals[1] * q.vals[3] + vals[2] * q.vals[0] - vals[0] * q.vals[2],
			vals[3] * q.vals[2] + vals[2] * q.vals[3] + vals[0] * q.vals[1] - vals[1] * q.vals[0],
			vals[3] * q.vals[3] - vals[0] * q.vals[0] - vals[1] * q.vals[1] - vals[2] * q.vals[2],
		};
	}
}
