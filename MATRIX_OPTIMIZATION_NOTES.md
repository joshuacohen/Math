# Matrix.h Optimization Notes

This file captures likely optimization opportunities in `inc/Matrix.h` without changing behavior.

## Highest-value opportunities

1. Replace `pow(x, 2)` in `magnitude_impl` with `x * x`.

Location: `inc/Matrix.h:303-304`

Why it matters:
- `pow` is much heavier than a multiply, even when the exponent is a constant.
- It also makes it harder for the compiler to fold or vectorize the sum-of-squares path.
- For vectors, this is one of the most likely runtime wins in the whole file.

Suggested shape:

```cpp
return std::sqrt((T{} + ... + (arr[Seq] * arr[Seq])));
```

2. Remove row/column temporary construction from generic matrix multiplication.

Location: `inc/Matrix.h:271-279`

Current behavior:
- `matrix_mul_impl` computes each output row by calling `matrix_mul_impl_inner`.
- `matrix_mul_impl_inner` builds a temporary `row(i)` and a temporary `val.col(Seq)` for every output element.

Why it matters:
- For an `M x N` by `N x P` multiply, this creates `M * P` column temporaries and repeatedly rebuilds the same left-hand row.
- The arithmetic cost is still dominant for large matrices, but for the fixed-size cases in this library the temporary traffic is a noticeable fraction of the work.

Better options:
- Best runtime path: transpose the right-hand operand once, then dot rows against rows from the transposed matrix.
- Best constexpr-first path: introduce a direct inner product helper that reads `data[row][k]` and `val.data[k][col]` without constructing `row_t` or `col_t` objects.

Sketch:

```cpp
template <class _T, size_t _W, size_t _H, size_t... K>
constexpr T mul_cell_impl(const Matrix<_T, _W, _H>& rhs, size_t row, size_t col, std::index_sequence<K...>) const {
    return (T{} + ... + (data[row][K] * rhs.data[K][col]));
}
```

This keeps the compile-time unrolling but removes the per-cell matrix temporaries.

3. Special-case determinant/adjoint/inverse for 3x3 and 4x4.

Location: `inc/Matrix.h:187-223`, `inc/Matrix.h:337-370`

Current behavior:
- Determinant expands by minors recursively.
- Adjoint computes every cofactor by building reduced matrices.
- Inverse is `adjoint() * (1 / det)`.

Why it matters:
- This is elegant and generic, but it creates many small temporary matrices and repeats overlapping subproblems.
- For `Mat3f` and especially `Mat4f`, closed-form formulas or a tuned elimination-based path will be much faster.

Practical recommendation:
- Keep the generic recursive implementation as a fallback.
- Add explicit fast paths for `2x2`, `3x3`, and `4x4` where the library is most likely to spend time.

4. Change normalize from divide-per-element to reciprocal multiply.

Location: `inc/Matrix.h:175-176`

Current behavior:

```cpp
return *this / length();
```

Why it matters:
- Scalar division is usually more expensive than multiplication.
- This performs `N` divisions after one square root.

Better shape:

```cpp
T inv_len = T{1} / length();
return *this * inv_len;
```

This is a small but reliable improvement.

5. Reduce temporary construction in transpose/remove-row/remove-column.

Locations:
- `inc/Matrix.h:313-332`
- `inc/Matrix.h:348-356`

Current behavior:
- These routines are already allocation-free, but they still construct intermediate row/column-shaped matrix values and dispatch through nested helpers.

Why it matters:
- The generated code is probably fine for tiny matrices, but these helpers are on the hot path for determinant, adjoint, and generic multiply.
- Simplifying the control flow helps both compile time and optimizer visibility.

Potential direction:
- Prefer a single direct assignment pass over nested helper composition where possible.

## Can fold expressions be vectorized?

Short answer:
- Not in a way you can rely on.

What the current code does well:
- Pack expansions over `index_sequence` force full unrolling at compile time.
- For very small fixed sizes like 2, 3, and 4, full unrolling is often already what you want.

What they do poorly for SIMD:
- A fold expression is not a loop. It becomes a scalar expression tree after instantiation.
- That means there is no explicit contiguous iteration space for the compiler to vectorize in the same way it would vectorize a simple counted loop.
- Compilers may still combine some operations, but that is opportunistic, target-dependent, and much less predictable than loop-based auto-vectorization.

Implication for this file:
- For `Vec3f`, `Vec4f`, `Mat3f`, and `Mat4f`, the current pack-expansion style is more likely to produce good scalar unrolled code than strong SIMD code.
- If you want deliberate SIMD, you probably need one of these approaches:

1. A runtime-only specialized path for float types using SIMD intrinsics or `std::experimental::simd` / `std::simd` when available.
2. A simple counted loop over `arr` for non-constexpr runtime paths, which gives the compiler a better auto-vectorization target.
3. Hand-specialized implementations for the hot fixed-size shapes, especially `Vec4f` and `Mat4f`.

Recommended compromise:
- Keep the current constexpr fold-based implementation as the generic baseline.
- Add specialized runtime fast paths for `float` vectors/matrices where benchmarking shows a benefit.

## Lower-priority opportunities

1. `scalar_add_impl`, `scalar_mul_impl`, `scalar_div_impl`, `vector_add_impl`, and `vector_sub_impl` all rebuild full result objects through pack expansion.

Location: `inc/Matrix.h:240-261`

This is clean and probably optimal enough for small fixed-size matrices. It only becomes interesting to revisit if you add explicit SIMD specializations.

2. `nearly_equal_impl` cannot short-circuit because the fold is instantiated as a single expression.

Location: `inc/Matrix.h:375-378`

That is not a meaningful performance issue for these sizes, but a loop could exit earlier on the first mismatch.

3. The `Xformf` default constructor assigns from global `Identity`.

Location: `inc/Matrix.h:393-394`

That likely optimizes away, but direct member initialization would express the intent more clearly and avoid depending on assignment in construction.

## Suggested order of attack

1. Replace `pow(arr[Seq], 2)` with `arr[Seq] * arr[Seq]`.
2. Rework generic matrix multiplication so each output cell reads directly from `data` instead of constructing `row()` and `col()` temporaries.
3. Change normalization to reciprocal multiply.
4. If benchmarks still show hot spots, add specialized `3x3`/`4x4` determinant and inverse implementations.
5. Only pursue explicit SIMD after benchmarking confirms scalar unrolled code is insufficient.

## Benchmark guidance

If you want to validate the above before changing APIs, benchmark these operations separately:

1. `Vec3f::length()` and `Vec4f::length()`
2. `Mat4f * Mat4f`
3. `Xformf * Xformf`
4. `Mat3f::inverse()`
5. `Mat4f::inverse()`

The likely result is:
- `pow` removal gives an immediate win.
- generic multiply cleanup helps codegen modestly.
- inverse/determinant specializations dominate any deeper speedup discussion.