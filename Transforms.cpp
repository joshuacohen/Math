#include <Transforms.h>
#include <cmath>

namespace Math3D {
	Xformf rotation(const Vec3f& axis, float angle) {
		float c = std::cos(angle);
		float s = std::sin(angle);
		float t = 1.0f - c;

		Vec3f n_axis = axis.normalize();
		float x = n_axis[0];
		float y = n_axis[1];
		float z = n_axis[2];

		float tx = t * x, ty = t * y, tz = t * z;
		float sx = s * x, sy = s * y, sz = s * z;
		float txx = tx * x, tyy = ty * y, tzz = tz * z;
		float txy = tx * y, txz = tx * z, tyz = ty * z;

		return Xformf {
			txx + c,  txy - sz, txz + sy,
			txy + sz, tyy + c,  tyz - sx,
			txz - sy, tyz + sx, tzz + c,
			0.0f,     0.0f,     0.0f,
		};
	}

	Xformf rotX(float angle) {
		float c = std::cos(angle);
		float s = std::sin(angle);

		return Xformf {
			1.0f, 	0.0f, 	0.0f,
			0.0f, 	c, 		-s,
			0.0f, 	s,		c,
			0.0f, 	0.0f,	0.0f,
		};
	}

	Xformf rotY(float angle) {
		float c = std::cos(angle);
		float s = std::sin(angle);

		return Xformf {
			c,		0.0f,	s,
			0.0f,	1.0f,	0.0f,
			-s, 	0.0f, 	c,
			0.0f,	0.0f,	0.0f,
		};
	}

	Xformf rotZ(float angle) {
		float c = std::cos(angle);
		float s = std::sin(angle);

		return Xformf {
			c,		-s,		0.0f,
			s,		c,		0.0f,
			0.0f,	0.0f,	1.0f,
			0.0f,	0.0f,	0.0f,
		};
	}

	Xformf translation(const Vec3f& offset) {
		return Xformf {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			offset[0], offset[1], offset[2],
		};
	}

	Xformf scale(const Vec3f& scale_factors) {
		return Xformf {
			scale_factors[0], 0.0f, 0.0f,
			0.0f, scale_factors[1], 0.0f,
			0.0f, 0.0f, scale_factors[2],
			0.0f, 0.0f, 0.0f,
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

	Mat4f orthographic(float width, float height, float scale, float offset) {
		return Mat4f {
			1.0f / width,	0.0f,			0.0f,					0.0f,
			0.0f,			1.0f / height,	0.0f,					0.0f,
			0.0f, 			0.0f, 			scale,					0.0f,
			0.0f, 			0.0f, 			1.0f - offset * scale,	1.0f,
		};
	}
}
