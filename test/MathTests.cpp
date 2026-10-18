#include <string>
#include <sstream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Transforms.h"
#include "GeometricPrimitives.h"

#include <numbers>

using namespace std;
using numbers::pi;
using namespace Math3D;

TEST_SUITE("Matrix") {

	TEST_CASE("Construction") {
		Vec3f a;
		Vec3f b(2.0f, 1.0f);
		Vec3f c(3.0f, 2.0f, 1.0f);

		CHECK(Mat3f({a, b, c}) == Mat3f {
			0.0f, 0.0f, 0.0f,
			2.0f, 1.0f, 0.0f,
			3.0f, 2.0f, 1.0f
		});

		CHECK(Xformf(1.2f) == Xformf {
			1.2f, 1.2f, 1.2f,
			1.2f, 1.2f, 1.2f,
			1.2f, 1.2f, 1.2f,
			1.2f, 1.2f, 1.2f,
		});

		CHECK(Xformf() == Identity);
		
		CHECK(Mat3f::identity() == Mat3f {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		});
	}

	TEST_CASE("Assignment") {
		Vec3f c = Vec3f{3.0f, 2.0f, 1.0f};
		CHECK(c == Vec3f{3.0f, 2.0f, 1.0f});
	}

	TEST_CASE("Identity") {
		CHECK(Vec3f{0.0f, 0.0f, 0.0f} == Vec3f{0.0f, 0.0f, 0.0f});
		CHECK(Vec3f{0.0f, 0.0f, 0.0f} != Vec3f{1.0f, 1.0f, 1.0f});

		CHECK(Mat3f() == Mat3f(
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f));

		CHECK(Mat3f() != Mat3f{
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,});
	}

	TEST_CASE("From Quaternion") {

		CHECK(from_quaternion(Quaternion(0.0f, 0.0f, 0.0f, 1.0f)) == Xformf::identity());
		CHECK(from_quaternion(Quaternion(0.0f, 1.0f, 0.0f, 0.0f)).nearly_equal(Xformf {
			-1.0f,  0.0f,  0.0f,
			 0.0f,  1.0f,  0.0f,
			 0.0f,  0.0f, -1.0f
		}));

		CHECK(from_quaternion(Quaternion(1.0f, 0.0f, 0.0f, 0.0f)).nearly_equal(rotX(pi)));
	}

	TEST_CASE("Access") {
		CHECK(Identity.row(2) == Vec3f(0.0f, 0.0f, 1.0f));
		CHECK(Identity.col(1) == Matrix<float, 1, 4>(0.0f, 1.0f, 0.0f, 0.0f));
	}

	TEST_CASE("Scalar addition") {
		CHECK(Vec3f{0.0f, 1.0f, 2.0f} + 1 == Vec3f{1.0f, 2.0f, 3.0f});
		CHECK((Vec3f{1.0f, 2.0f, 3.0f} +=2) == Vec3f{3.0f, 4.0f, 5.0f});
		CHECK(++Vec3f() == Vec3f{1.0f, 1.0f, 1.0f});
	}

	TEST_CASE("Scalar multiplication") {
		CHECK(Vec3f(1.0f, 2.0f, 3.0f) * 3.0f == Vec3f(3.0f, 6.0f, 9.0f));
		CHECK(Vec3f(0.0f, 1.0f, -2.0f) * -2.5f == Vec3f(0.0f, -2.5f, 5.0f));
	}

	TEST_CASE("Vector addition") {
		CHECK(Vec3f(1.0f, 1.0f, 1.0f) + Vec3f{2.0f, 2.0f, 2.0f} == Vec3f{3.0f, 3.0f, 3.0f});

		Vec3f result(2.0f, 4.0f, 6.0f);
		result += Vec3f(1.0f, 2.0f, 3.0f);
		CHECK(result == Vec3f(3.0f, 6.0f, 9.0f));
	}

	TEST_CASE("Vector subtraction") {
		CHECK(Vec3f(1.0f, 2.0f, 3.0f) - Vec3f(3.0f, 2.0f, 1.0f) == Vec3f(-2.0f, 0.0f, 2.0f));
	}

	TEST_CASE("Matrix multiplication") {
		CHECK( Matrix<int, 3, 3>  {1, 2, 3, 3, 2, 1, 1, 2, 3}
			*  Matrix<int, 3, 3>  {4, 5, 6, 6, 5, 4, 4, 6, 5}
			== Matrix<int, 3, 3>  {28, 33, 29, 28, 31, 31, 28, 33, 29});

		// Xformf * Xformf (treated as 4x4 with implied column (0,0,0,1))
		Xformf a {
			1.0f, 0.0f, 0.0f,  // right
			0.0f, 1.0f, 0.0f,  // up
			0.0f, 0.0f, 1.0f,  // forward
			1.0f, 2.0f, 3.0f,  // position (translation)
		};
		Xformf b {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			4.0f, 5.0f, 6.0f,
		};

		CHECK(Xformf {
			1.0f, 2.0f, 3.0f,
			3.0f, 2.0f, 1.0f,
			1.0f, 2.0f, 3.0f,
			0.0f, 0.0f, 0.0f,
		} * Xformf {
			4.0f, 5.0f, 6.0f,
			6.0f, 5.0f, 4.0f,
			4.0f, 6.0f, 5.0f,
			0.0f, 0.0f, 0.0f,
		} == Xformf {
			28.0f, 33.0f, 29.0f,
			28.0f, 31.0f, 31.0f,
			28.0f, 33.0f, 29.0f,
			0.0f, 0.0f, 0.0f,
		});

		CHECK(Xformf {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 2.0f, 3.0f,
		} * Mat4f {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		} == Mat4f {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 2.0f, 3.0f, 1.0f,
		});

		// Two translations should combine: (1,2,3) + (4,5,6) = (5,7,9)
		CHECK(a * b == Xformf {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			5.0f, 7.0f, 9.0f,
		});
	}

	TEST_CASE("Operator *= (Matrix)") {
		Matrix<int, 3, 3>  a {
			1, 2, 3,
			3, 2, 1,
			1, 2, 3
		};

		Matrix<int, 3, 3> b {
		   4, 5, 6,
		   6, 5, 4,
		   4, 6, 5
		};

		CHECK((a *= b) == Matrix<int, 3, 3> {28, 33, 29, 28, 31, 31, 28, 33, 29});
	}

	TEST_CASE("Scalar division") {
		CHECK(Vec3f(3.0f, 6.0f, 9.0f) / 3.0f == Vec3f(1.0f, 2.0f, 3.0f));
	}

	TEST_CASE("Operator[]") {
		CHECK(Vec3f(1.0f, 2.0f, 3.0f)[1] == 2.0f);

		Xformf i = Identity;
		Vec3f v = i[2];

		CHECK(v == Vec3f(0.0f, 0.0f, 1.0f));
		CHECK(v[2] == 1.0f);

		i[3] = Vec3f(7.0f, 8.0f, 9.0f);
		i[2][2] = 3.0f;

		CHECK(i == Xformf {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 3.0f,
			7.0f, 8.0f, 9.0f
		});
	}

	TEST_CASE("Inner Product") {
		CHECK(Vec3f(1.0f, 2.0f, 3.0f).dot(Vec3f(3.0f, 4.0f, 5.0f)) == 26.0f);
	}

	TEST_CASE("Interpolation") {
		CHECK(lerp(0.0f, 10.0f, 0.7f) == 7.0f);
		CHECK(Vec3f(2.0f, 4.0f, 6.0f).lerp(Vec3f(4.0f, 8.0f, 12.0f), 0.5f) == Vec3f(3.0f, 6.0f, 9.0f));
	}
	TEST_CASE("Cross Product") {
		CHECK(Vec3f(1.0f, 2.0f, 3.0f).cross(Vec3f(3.0f, 4.0f, 5.0f)) == Vec3f(-2.0f, 4.0f, -2.0f));
	}

	TEST_CASE("Length") {
		CHECK(Vec3f(3.0f, 4.0f, 0.0f).length() == 5);
	}

	TEST_CASE("Normalization") {
		CHECK(Vec3f(3.0f, 4.0f, 0.0f).normalize() == Vec3f(3.0f / 5.0f, 4.0f / 5.0f, 0.0f));
	}

	TEST_CASE("Column Removal") {
		Mat4f before {
			1.0f, 2.0f, 3.0f, 4.0f,
			2.0f, 3.0f, 4.0f, 1.0f,
			3.0f, 4.0f, 1.0f, 2.0f,
			4.0f, 1.0f, 2.0f, 3.0f,
		};

		Xformf after {
			1.0f, 2.0f, 4.0f,
			2.0f, 3.0f, 1.0f,
			3.0f, 4.0f, 2.0f,
			4.0f, 1.0f, 3.0f,
		};

		CHECK(before.remove_column(2) == after);
	}

	TEST_CASE("Row Removal") {
		Mat3f before {
			1.0f, 3.0f, 2.0f,
			4.0f, 1.0f, 3.0f,
			2.0f, 5.0f, 2.0f,
		};

		Matrix<float, 3, 2> after {
			4.0f, 1.0f, 3.0f,
			2.0f, 5.0f, 2.0f,
		};

		CHECK(before.remove_row(0) == after);
	}

	TEST_CASE("Determinant") {
		CHECK(Mat2f {
			3.0f, 8.0f,
			4.0f, 6.0f,
		}.determinant() == -14);

		CHECK(Mat3f {
			1.0f, 3.0f, 2.0f,
			4.0f, 1.0f, 3.0f,
			2.0f, 5.0f, 2.0f,
		}.determinant() == 17);

		CHECK(Mat4f {
			2.0f, 1.0f, 3.0f, 4.0f,
			0.0f, -1.0f, 2.0f, 1.0f,
			3.0f, 2.0f, 0.0f, 5.0f,
			-1.0f, 3.0f, 2.0f, 1.0f,
		}.determinant() == 35);
	}

	TEST_CASE("Trace") {
		CHECK(
			Mat3f {
				1.0f, 2.0f, 3.0f,
				4.0f, -5.0f, 6.0f,
				7.0f, 8.0f, 9.0f,
			}.trace() == Vec3f(1.0f, -5.0f, 9.0f)
		);
	}

	TEST_CASE("Sum Trace") {
		CHECK(
			Mat3f {
				1.0f, 2.0f, 3.0f,
				4.0f, -5.0f, 6.0f,
				7.0f, 8.0f, 9.0f,
			}.sum_trace() == 5.0f
		);
	}

	TEST_CASE("Transpose") {
		CHECK(
			Matrix<int, 3, 2> {
				2, 5, 3,
				4, 7, 0,
			}.transpose() == 
			Matrix<int, 2, 3> {
				2, 4,
				5, 7,
				3, 0,
			}
		);
	}

	TEST_CASE("Reflect") {
		CHECK(
			Vec3f(1.0f, 2.0f, 3.0f).reflect(Vec3f(0.0f, 1.0f, 0.0f)) == Vec3f(1.0f, -2.0f, 3.0f)
		);
	}

	TEST_CASE("Reject") {
		CHECK(
			Vec3f(1.0f, 2.0f, 3.0f).reject(Vec3f(0.0f, 1.0f, 0.0f)) == Vec3f(1.0f, 0.0f, 3.0f)
		);
	}

	TEST_CASE("Adjoint") {
		Matrix<int, 3, 3> before {
			-1, -2, -2,
			2, 1, -2,
			2, -2, 1
		};

		Matrix<int, 3, 3> after {
			-3, 6, 6,
			-6, 3, -6,
			-6, -6, 3
		};

		CHECK(before.adjoint() == after);
	}

	TEST_CASE("Adjoint 2x2") {
		Mat2f mat {
			3.0f, 8.0f,
			4.0f, 6.0f,
		};

		Mat2f expected {
			6.0f, -8.0f,
			-4.0f, 3.0f,
		};

		CHECK(mat.adjoint() == expected);
	}

	TEST_CASE("Adjoint Identity") {
		// adj(I) = I
		Mat3f identity {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
		};

		CHECK(identity.adjoint() == identity);
	}

	TEST_CASE("Adjoint 4x4") {
		Mat4f mat {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 3.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 4.0f,
		};

		// For diagonal matrices, adjoint is also diagonal with pattern:
		// adj(D)[i,i] = det(D) / D[i,i]
		// det(D) = 1*2*3*4 = 24
		Mat4f expected {
			24.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 12.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 8.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 6.0f,
		};

		CHECK(mat.adjoint() == expected);
	}

	// TODO adjoint property
	TEST_CASE("Inverse") {
		Mat3f before {
			4.0f, 3.0f, 8.0f,
			6.0f, 2.0f, 5.0f,
			1.0f, 5.0f, 9.0f
		};

		Mat3f inv = before.inverse();

		Mat3f after {
			-(1.0f/7.0f), 13.0f/49.0f,    -(1.0f/49.0f),
			-1.0f, 		  4.0f/7.0f,      4.0f/7.0f,
			4.0f/7.0f,    -(17.0f/49.0f), -(10.0f/49.0f)
		};

		CHECK(inv.nearly_equal(after));
		CHECK(Mat3f { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,}.nearly_equal(before * before.inverse()));
	}

	TEST_CASE("Swizzle") {
		CHECK(Vec3f(1.0f, 2.0f, 3.0f).swizzle<0, 1, 2>() == Vec3f(1.0f, 2.0f, 3.0f));
		CHECK(Vec3f(1.0f, 2.0f, 3.0f).swizzle<2, 0, 1>() == Vec3f(3.0f, 1.0f, 2.0f));
		CHECK(Mat3f {
			1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f,
		}.swizzle<8, 7, 6, 5, 4, 3, 2, 1, 0>() == Mat3f {
			9.0f, 8.0f, 7.0f,
			6.0f, 5.0f, 4.0f,
			3.0f, 2.0f, 1.0f,
		});
	}

	TEST_CASE("Operator<<") {
		stringstream vec, mat;

		vec << Vec3f(1.0f, 0.0f, 0.0f);
		CHECK(vec.str() == "1, 0, 0");

		mat << Xformf::identity();
		CHECK(mat.str() =="1, 0, 0\n0, 1, 0\n0, 0, 1\n0, 0, 0");
	}
}

TEST_SUITE("Transforms") {
	Xformf rot90z {  // 90 degree rotation around Z
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
	};

	Xformf trans {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
	};

	Xformf scaling {
		2.0f, 0.0f, 0.0f,
		0.0f, 3.0f, 0.0f,
		0.0f, 0.0f, 4.0f,
		0.0f, 0.0f, 0.0f,
	};

	TEST_CASE("Rotation") {
		// Test rotation * translation
		CHECK(nearly_equal(rot90z, rotation(Vec3f(0.0f, 0.0f, 1.0f), 3.14159265f / 2.0f)));
	}

	TEST_CASE("Translation") {
		CHECK(nearly_equal(trans, translation(Vec3f(1.0f, 0.0f, 0.0f))));
	}

	TEST_CASE("Scaling") {
		CHECK(nearly_equal(scaling, scale(Vec3f(2.0f, 3.0f, 4.0f))));
	}

	TEST_CASE("Composition") {
		// rot90z * trans: rotation followed by translation
		CHECK(rot90z * trans == Xformf {
			0.0f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,
		});

		CHECK(rot90z * trans * scaling == Xformf {
			0.0f, -3.0f, 0.0f,
			2.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 4.0f,
			2.0f, 0.0f, 0.0f,
		});
	}

	TEST_CASE("Decomposition") {
		Xformf a {
			0.0f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,
		};
		Quaternion localRot;
		Vec3f localTrans, localScale;

		decompose(a, localRot, localTrans, localScale);
		CHECK(nearly_equal(localRot, rot90z));

		CHECK(nearly_equal(localTrans, Vec3f(1.0f, 0.0f, 0.0f)));

		Xformf b {
			0.0f, -3.0f, 0.0f,
			2.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 4.0f,
			2.0f, 0.0f, 0.0f,
		};
		decompose(b, localRot, localTrans, localScale);
		
		CHECK(nearly_equal(localRot, rot90z));
		CHECK(nearly_equal(localTrans, Vec3f(2.0f, 0.0f, 0.0f)));
		CHECK(nearly_equal(localScale, Vec3f(2.0f, 3.0f, 4.0f)));
	}

	TEST_CASE("Look At") {
		CHECK(nearly_equal(look_at(translation(Vec3f{0.0f, 0.0f, -1.0f}), Identity), Xformf {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
		}));
	}

	TEST_CASE("Perspective Projection") {

		// Row major
		CHECK(nearly_equal(perspective((float)pi / 2.0f, 16.0f / 9.0f, 0.1f, 100.0f), Mat4f {
			0.5625f, 0.0f,  0.0f,       0.0f,
			0.0f,    1.0f,  0.0f,       0.0f,
			0.0f,    0.0f, 1.0010010f,  1.0f,
			0.0f,    0.0f, -0.1001001f, 0.0f,
		}));
	}

	TEST_CASE("Euclidean Distance") {
		CHECK(distance(Vec3f(1.0f, 2.0f, 3.0f), Vec3f(4.0f, 6.0f, 3.0f)) == 5.0f);
		CHECK(distance(Vec3f(-1.0f, -2.0f, -3.0f), Vec3f(-4.0f, -6.0f, -3.0f)) == 5.0f);
		CHECK(distance(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f)) == 0.0f);
	}

	TEST_CASE("Angle Between Vectors") {
		CHECK(angle(Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f)) == doctest::Approx(pi / 2.0f));
		CHECK(angle(Vec3f(1.0f, 0.0f, 0.0f), Vec3f(1.0f, 0.0f, 0.0f)) == doctest::Approx(0.0f));
		CHECK(angle(Vec3f(1.0f, 0.0f, 0.0f), Vec3f(-1.0f, 0.0f, 0.0f)) == doctest::Approx(pi));
	}

	TEST_CASE("MVP") {
		Xformf model = Identity;
		Xformf view = look_at(translation(Vec3f{0.0f, 0.0f, -1.0f}), Identity);
		Mat4f projection = perspective((float)pi, 1.778f, 0.1f, 100.0f);
		
		Mat4f expected {
			0.0f, 0.0f, 0.0f, 0.0f, 
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.001001f, 1.0f,
			0.0f, 0.0f, 0.9009009f, 1.0f,
		};

		CHECK(nearly_equal(model * view * projection, expected));
	}
}

TEST_SUITE("Quaternions") {
	TEST_CASE("Initialization") {
		Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);

		CHECK(q1.i == 1.0f);
		CHECK(q1.j == 2.0f);
		CHECK(q1.k == 3.0f);
		CHECK(q1.r == 4.0f);

		Quaternion q2(Vec3f(1.0f, 0.0f, 0.0f), pi / 2.0f); // axis-angle constructor
		CHECK(q2.nearly_equal(Quaternion(sqrt(0.5f), 0.0f, 0.0f, sqrt(0.5f))));
	}

	TEST_CASE("Equality") {
		Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);
		Quaternion q2(1.0f, 2.0f, 3.0f, 4.0f);
		Quaternion q3(4.0f, 3.0f, 2.0f, 1.0f);

		CHECK(q1 == q2);
		CHECK(!(q1 == q3));
	}

	TEST_CASE("From Rotation Matrix") {
		CHECK(
			Quaternion(rotX(pi / 2.0f))
			.nearly_equal(Quaternion(sqrt(0.5f), 0.0f, 0.0f, sqrt(0.5f)))
		);
	}

	TEST_CASE("Linear Interpolation") {
		CHECK(lerp(
			Quaternion(0.0f, 0.0f, 0.0f, 1.0f),
			Quaternion(1.0f, 0.0f, 0.0f, 0.0f), 
			0.5f
		).nearly_equal(Quaternion(0.5f, 0.0f, 0.0f, 0.5f)));
	}

	TEST_CASE("Spherical Linear Interpolation") {
		CHECK(slerp(
			Quaternion(0.0f, 0.0f, 0.0f, 1.0f),
			Quaternion(1.0f, 0.0f, 0.0f, 0.0f), 
			0.5f
		).nearly_equal(Quaternion(sqrt(2)/2, 0.0f, 0.0f, sqrt(2)/2)));
	}

	TEST_CASE("To Euler") {
		CHECK(Quaternion(rotX(3.14159265f / 2.0f)).Euler().nearly_equal(Vec3f(3.14159265f / 2.0f, 0.0f, 0.0f)));
	}
}

TEST_SUITE("Collision Detection") {
	TEST_CASE("Half Space 3D") {
		Plane plane { Vec3f(0.0f, 1.0f, 0.0f), 0.0f }; // horizontal plane at y=0
		CHECK(HalfSpace3D(Vec3f(0.0f, 1.0f, 0.0f), plane) > 0); // above the plane
		CHECK(HalfSpace3D(Vec3f(0.0f, -1.0f, 0.0f), plane) < 0); // below the plane
		CHECK(HalfSpace3D(Vec3f(1.0f, 0.0f, 1.0f), plane) == 0); // on the plane
	}
}
