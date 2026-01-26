#pragma once
#include "Matrix.h"

namespace Math3D {
	using Point = Vec3f;

	struct Vert2d {
		Vec2f pos;
		Vec3f col;
		Vec2f uv;
	};

	struct Vert3d {
		Vec3f pos;
		Vec3f norm;
		Vec2f uv;
	};

	struct Tri2d {
		Vert2d verts[3];
	};

	struct Tri3d {
		Vert3d verts[3];
	};

	struct Ray {
		Point point;
		Vec3f dir;
	};

	struct Plane {
		Vec3f normal;
		float distance;
	};

	struct Sphere {
		Point center;
		float radius;
	};

	struct AABB {
		Point center;
		float halfwidths[3];
	};
}
