#include <Transforms.h>
#include <cmath>

namespace Math3D {
	Xformf translation(const Vec3f& offset) {
		return Xformf {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			offset[0], offset[1], offset[2],
		};
	}

	Mat4f perspective(float fov, float aspect, float near_clip, float far_clip) {
		assert(far_clip != near_clip);
		assert(fov != 0.0f);

		float yScale = 1.0f / std::tan(fov / 2.0f);
		float clip = far_clip / (far_clip - near_clip);

		return Mat4f {
			yScale / aspect,	0.0f, 	0.0f, 	0.0f,
			0.0f,				yScale, 0.0f,	0.0f,
			0.0f, 				0.0f, 	clip, 	1.0f,
			0.0f, 		0.0f, -near_clip * clip, 0.0f,
		};
	}

	Xformf look_at(const Xformf& from, const Xformf& to) {
		Vec3f eye = from[3];
		Vec3f target = to[3];

		Vec3f fwd = (target - eye).normalize();
		Vec3f left = Vec3f(0.0f, 1.0f, 0.0f).cross(fwd).normalize();
		Vec3f up = fwd.cross(left).normalize();

		return Xformf { {left, up, fwd, eye} };

	}
}
