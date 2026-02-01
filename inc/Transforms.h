#pragma once
#include "Matrix.h"
#include <cmath>

namespace Math3D {
	Xformf translation(const Vec3f& offset);
	Xformf scale(const Vec3f& scale_factors);

	Xformf look_at(const Xformf& from, const Xformf& to);


	Mat4f perspective(float fov, float aspect, float near_clip, float far_clip);
	Mat4f orthographic(float width, float height, float scale, float offset);
}
