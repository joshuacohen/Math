#include <string>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Matrix.h"

TEST_SUITE("Matrix") {
	using namespace Math3D;

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
	}

	TEST_CASE("Vector subtraction") {
		CHECK(Vec3f(1.0f, 2.0f, 3.0f) - Vec3f(3.0f, 2.0f, 1.0f) == Vec3f(-2.0f, 0.0f, 2.0f));
	}

	TEST_CASE("Matrix multiplication") {
		CHECK( Matrix<int, 3, 3>  {1, 2, 3, 3, 2, 1, 1, 2, 3}
			*  Matrix<int, 3, 3>  {4, 5, 6, 6, 5, 4, 4, 6, 5}
			== Matrix<int, 3, 3>  {28, 33, 29, 28, 31, 31, 28, 33, 29});
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
		CHECK(Vec3f(1.0f, 2.0f, 3.0f).cross(Vec3f(3.0f, 4.0f, 5.0f)) == Vec3f(2.0f, -4.0f, 2.0f));
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
			}.trace() == 5.0f
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

	// TODO epsilon
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
}
