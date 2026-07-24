#pragma once
#include <cmath>

// This is some bullshit
// #define EPSILON numeric_limits<T>::epsilon()
#define EPSILON 1e-06F

namespace Math3D {
	template <typename T>
	constexpr T lerp(const T& a, const T& b, float t) {
		return (1 - t) * a + t * b;
	}

	template <typename T>
	bool nearly_equal(const T& lhs, const T& rhs) {
		return std::abs(lhs - rhs) < EPSILON;
	}
}