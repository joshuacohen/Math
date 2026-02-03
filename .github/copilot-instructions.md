# Math Library Coding Instructions

This document provides AI assistant guidelines specific to the math library implementation.

## Architecture & Type System

**Core Types** (defined in `inc/`):
- `Mat<T, R, C>` — Generic matrix with row-major storage
  - Specializations: `Vec2f`, `Vec3f`, `Vec4f`, `Vec3d`, etc.
  - Specializations: `Mat2f`, `Mat3f`, `Mat4f` (square types)
  - Supports: scalar ops, dot/cross product, normalization, transpose, determinant, inverse, adjoint
- `Xformf` — Affine transform (Mat4f specialization for homogeneous coords)
- `Quaternion` — Unit quaternion for rotations
  - Supports: normalization, conjugate, Slerp, (coming) Euler angles, matrix conversion

**Row-Major Layout**:
```cpp
// Internal storage: row-major (matches C++ array semantics)
Mat4f m;
m[0] = row_0;  // First row
m[1] = row_1;  // Second row
// For D3D12/Vulkan shaders: document transposition requirements
```

## Coding Patterns

### 1. Constexpr-First Implementation

All new operations should be `constexpr` unless prohibitively complex:

```cpp
// In inc/Vector.h
template <typename T, size_t N>
constexpr T dot(const Vec<T, N>& a, const Vec<T, N>& b) {
    T result{};
    [...]<size_t... I>(std::index_sequence<I...>) {
        result = ((a[I] * b[I]) + ...);
    }(std::make_index_sequence<N>{});
    return result;
}
```

**Rationale**: Enable compile-time matrix evaluation for constants, avoid runtime overhead.

### 2. Index Sequences for Vectorization

Use `std::index_sequence` for compile-time loop unrolling:

```cpp
// Pattern: use Seq_Data, Seq_Row, Seq_Col helpers
template <typename T, size_t R, size_t C, size_t... II>
constexpr auto transpose_impl(const Mat<T, R, C>& m, std::index_sequence<II...>) {
    return Mat<T, C, R>{ m.col(II)... };
}

template <typename T, size_t R, size_t C>
constexpr auto transpose(const Mat<T, R, C>& m) {
    return transpose_impl(m, std::make_index_sequence<R>{});
}
```

### 3. Union-Based Flexible Access

Provide multiple access patterns:

```cpp
// In inc/Matrix.h
		union {
			array<T, N> arr;
			array<array<T, W>, H> data;
			conditional_t<H == 1, array<T, W>, array<row_t, H>> vec;
		};
```

### 4. Type Aliases for Readability

Define common specializations:

```cpp
// In inc/Matrix.h
using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;
using Mat2f = Mat<float, 2, 2>;
using Mat3f = Mat<float, 3, 3>;
using Mat4f = Mat<float, 4, 4>;
using Xformf = Mat4f;  // Affine transformation
```

### 5. Static Helpers for Complex Operations

Place non-member functions in namespace (not class methods):

```cpp
namespace math {
    // Good: static helper in namespace
    constexpr Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
        // ...
    }
    
    // Constructor: only for primary conversion
    Quaternion(const Xformf& rotMatrix);  // Rotation part only
}
```

**Rationale**: Keep class interface minimal; reserve methods for core operations.

### 6. Requires Clauses for Specialization

Use `requires` to specialize for specific types:

```cpp
// Only for 3D vectors
template <typename T>
requires(std::is_same_v<T, Vec<float, 3>> || std::is_same_v<T, Vec<double, 3>>)
constexpr auto cross(const T& a, const T& b) {
    return T{ a.y * b.z - a.z * b.y, ... };
}
```

## Testing Practices

### Unit Tests (`test/MathTests.cpp`)

**Pattern**: Doctest inline with critical math operations

```cpp
#include <doctest/doctest.h>

TEST_CASE("Matrix Inverse") {
    Mat4f m = /* ... */;
    Mat4f inv = m.Inverse();
    Mat4f identity = m * inv;
    CHECK(identity == Mat4f::Identity());  // With epsilon tolerance
}
```

**Coverage Goals**:
- Edge cases: zero vectors, singular matrices, degenerate quaternions
- Numerical stability: large/small values, inverse accuracy
- Round-trip conversions: Euler → Quat → Euler, Matrix → Quat → Matrix

<!-- ### Benchmark Tests (`test/MathBenchmarks.cpp`, when added)

```cpp
#include <benchmark/benchmark.h>

BENCHMARK(Quaternion_Slerp) {
    auto q1 = Quaternion::Identity();
    auto q2 = Quaternion(Vec3f{1,0,0}, 90.0f);
    for(auto _ : state) {
        benchmark::DoNotOptimize(Quaternion::Slerp(q1, q2, 0.5f));
    }
}
``` -->

## Common Gotchas

### 1. Row-Major vs Column-Major Confusion

**Math library**: Row-major (C++ matrix semantics)
```cpp
mat[0] = {m00, m01, m02, m03};  // First row
mat[1] = {m10, m11, m12, m13};  // Second row
```

**HLSL (D3D12 default)**: Column-major
```hlsl
// Needs transpose in shader or CPU-side multiplication order
```

**Action**: Document in shader integration; always comment when transposing.

<!-- ### 2. Quaternion Normalization

Always normalize quaternions before Slerp:
```cpp
// Bad: assumes normalized input
Quaternion::Slerp(q1, q2, t);

// Good: normalize if uncertain
q1 = q1.Normalize();
q2 = q2.Normalize();
Quaternion::Slerp(q1, q2, t);
``` -->

### 3. Constexpr Sqrt Issues

`std::sqrt` is not `constexpr` in older C++ standards. Use workaround:
```cpp
// Avoid constexpr sqrt in pre-C++26 standards
constexpr float magnitude_sq(const Vec3f& v) {  // OK
    return dot(v, v);
}

float magnitude(const Vec3f& v) {  // Not constexpr
    return std::sqrt(dot(v, v));
}
```

<!-- ### 4. Gimbal Lock in Euler Angles

Document convention in `Quaternion::ToEuler()`:
```cpp
// Document: ZYX Euler order, returns angles in radians
// Gimbal lock occurs when pitch ≈ ±π/2 → yaw and roll become dependent
struct EulerAngles { float pitch, yaw, roll; };
EulerAngles ToEuler() const;  // ZYX order assumed
``` -->

## File Organization

```
src/math/
├── inc/
│   ├── Matrix.h        — Mat<T,R,C>, inverse, transpose, proj/view builders
│   ├── Quaternion.h    — Unit quaternion, Slerp, Euler, matrix conversion
│   └── GeometricPrimitives.h — Plane, Ray, AABB (future)
├── Transforms.cpp      — (minimal; most inline)
├── Quaternion.cpp      — Complex operations (FromEuler, ToEuler, Slerp impl)
├── Geometry.cpp        — (future) Plane/Ray/AABB implementations
├── test/
│   ├── MathTests.cpp   — Unit + integration tests (doctest)
├── CHANGELOG.md        — Release notes, completed features
└── MATH_LIBRARY_ANALYSIS.md — Implementation roadmap
```

## Dependency Management

**Within Math Library** (acyclic):
1. Matrix.h — no deps (fundamental)
2. Transforms.h — depends on Matrix.h
3. Quaternion.h — depends on Matrix.h (for conversions)
4. GeometricPrimitives.h — depends on Vector.h, Matrix.h

**External Dependencies**:
- `<cmath>` — sqrt, sin, cos, asin, atan2 (for non-constexpr operations)
- `<array>`, `<index_sequence>` — std lib containers and sequences
- `<doctest/doctest.h>` — inline testing (test files only)
- `<benchmark/benchmark.h>` — performance testing (benchmark files only)

## Performance Targets

| Operation | Target | Notes |
|-----------|--------|-------|
| Vec3f dot product | <5 cycles | Constexpr unrolled |
| Mat4f multiply | <50 cycles | 64 FLOPs, memory-bound |
| Mat4f inverse | <200 cycles | Adjoint method, determinant |
| Quaternion Slerp | <50 cycles | Normalized inputs assumed |
| LookAt (camera matrix) | <100 cycles | Cache result if camera static |

## Integration with Engine

**When Using Math Library** (`D3D12App`, `VKApp`):

```cpp
// In D3D12App.cpp
#include "src/math/inc/Matrix.h"
#include "src/math/inc/Quaternion.h"

// Example: set camera once per frame
Mat4f view = math::LookAt(eye, center, up);
Mat4f proj = math::Perspective(fov_deg, aspect, near, far);
Mat4f viewProj = proj * view;  // Apply in correct order

// Cache if eye/center don't change
static Mat4f cached_view;
if (camera_moved) {
    cached_view = math::LookAt(eye, center, up);
}
```

---

## When to Ask for Help

Ask the AI assistant when:
- Adding a new matrix operation (document row-major semantics)
- Implementing Euler angle conversion (clarify gimbal lock handling)
- Optimizing hot-path functions (profiling context needed)
- Integrating with shaders (confirm transposition requirements)

---

## See Also

- [MATH_LIBRARY_ANALYSIS.md](MATH_LIBRARY_ANALYSIS.md) — Implementation roadmap
- [CHANGELOG.md](CHANGELOG.md) — Completed work and version history
- [.github/copilot-instructions.md](.github/copilot-instructions.md) — Engine-wide guidelines
