#pragma once
#include "Matrix.h"
#include <cmath>

namespace Math3D {
	Mat4f perspective(float fov, float aspect, float near_clip, float far_clip);
	Xformf look_at(const Xformf& from, const Xformf& to);
}
