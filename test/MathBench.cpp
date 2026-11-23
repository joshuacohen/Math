#include "Matrix.h"
#include "doctest.h"
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

using namespace Math3D;
#include "DirectXMath.h"

TEST_SUITE("Benchmarks") {
	TEST_CASE("Addition") {
		Vec<float, 3> a(1.0f, 2.0f, 3.0f);
		Vec<float, 3> b(4.0f, 5.0f, 6.0f);
		DirectX::XMVECTOR xa = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
		DirectX::XMVECTOR xb = DirectX::XMVectorSet(4.0f, 5.0f, 6.0f, 0.0f);

		constexpr size_t BatchSize = 1000;
		ankerl::nanobench::Bench bench;
		bench.relative(true).batch(BatchSize).unit("add");

		bench.run("DXMath Equivalent", [&] {
			auto acc = xa;
			ankerl::nanobench::doNotOptimizeAway(xa);
			ankerl::nanobench::doNotOptimizeAway(xb);
			ankerl::nanobench::doNotOptimizeAway(acc);

			for (size_t i = 0; i < BatchSize; ++i) {
				acc = DirectX::XMVectorAdd(acc, xb);
			}
		});

		bench.run("Add 2 Vectors", [&] {
			auto acc = a;
			ankerl::nanobench::doNotOptimizeAway(a);
			ankerl::nanobench::doNotOptimizeAway(b);
			ankerl::nanobench::doNotOptimizeAway(acc);

			for (size_t i = 0; i < BatchSize; ++i) {
				acc += b;
			}
		});
	}
}