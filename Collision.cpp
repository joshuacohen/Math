#include "GeometricPrimitives.h"

namespace Math3D {
	float HalfSpace3D(const Point& lhs, const Plane& rhs) {
		return lhs.dot(rhs.n) - rhs.d;
	}
}