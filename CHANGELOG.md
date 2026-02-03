# Math Library Changelog

## Completed Features

### Matrix System (`Matrix.h`)
- ✅ Generic `Matrix<T, W, H>` template using constexpr index sequences
- ✅ Vector aliases: `Vec2f`, `Vec3f`, `Vec4f`
- ✅ Transformation matrix: `Xformf` (3×4 for position + rotation)
- ✅ Core operations:
  - Scalar & vector addition/subtraction/division
  - Matrix multiplication (general case, compile-time dimensions checked)
  - Dot product (element-wise inner product)
  - Cross product (3D only)
  - Magnitude & normalization
  - Linear interpolation (lerp)
  - Trace (sum of diagonal for square matrices)
  - Row/column access
  - **Transpose** — constexpr with index sequences
  - **Determinant** — recursive cofactor expansion; specialized for 1×1, 2×2
  - **Adjoint** — computed via cofactor matrix
  - **Inverse** — using cofactor/adjoint method for any square matrix

### Quaternion System (`Quaternion.h`/`.cpp`)
- ✅ Four-component quaternion `(i, j, k, r)` representation
- ✅ Constructors: default, component-based, from axis-angle, from rotation matrix
- ✅ Operations: multiply, divide, add, subtract (both quaternion & scalar)
- ✅ Conversions: `ToRot()` to 3×3 rotation matrix
- ✅ Utilities:
  - `Dot()` — const correct
  - `Mag()` — const correct
  - `Slerp()` — spherical linear interpolation
  - **Normalize()** — returns normalized copy
  - **Conjugate()** — flips i, j, k components

### Geometric Primitives (`GeometricPrimitives.h`)
- ✅ 2D vertex struct: `Vert2d` (position, color, UV)
- ✅ 3D vertex struct: `Vert3d` (position, normal, UV)
- ✅ Triangle containers: `Tri2d`, `Tri3d`

### Test Coverage (`test/MathTests.cpp`)
- ✅ Construction and assignment
- ✅ Scalar operations (addition, multiplication, division)
- ✅ Vector operations (addition, dot product, cross product)
- ✅ Matrix multiplication
- ✅ Row/column removal
- ✅ Determinant (2×2, 3×3, 4×4)
- ✅ Trace
- ✅ **Transpose** — involution and row/col swap
- ✅ **Adjoint** — for 2×2, 3×3, 4×4 matrices
- ✅ **Inverse** — validation via `M · M^-1 ≈ I` with epsilon tolerance
- ✅ Length and normalization
- ✅ Interpolation

## Known Limitations & To-Do Items

See [../../../MATH_LIBRARY_ANALYSIS.md](../../../MATH_LIBRARY_ANALYSIS.md) for remaining work and implementation plan.
