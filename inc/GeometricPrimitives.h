#pragma once
#include "Matrix.h"

struct Vert2d {
	Math3D::Vec2f pos;
	Math3D::Vec3f col;
	Math3D::Vec2f uv;
};

struct Vert3d {
	Math3D::Vec3f pos;
	Math3D::Vec3f norm;
	Math3D::Vec2f uv;
};

struct Tri2d {
	Vert2d verts[3];
};

struct Tri3d {
	Vert3d verts[3];
};
