# Math Library Implementation TODO

For completed work, see [src/math/CHANGELOG.md](src/math/CHANGELOG.md)

## Critical Gaps (Remaining Work)

| Feature | Priority | Est. Time | Notes |
|---------|----------|-----------|-------|
| **Perspective/Orthographic Projection** | CRITICAL | 1-2 hrs | Camera setup builders |
| **LookAt (View Matrix)** | CRITICAL | 1 hr | Standard camera construction |
| **Vector Distance/Angle** | HIGH | 30 min | Euclidean distance, angle between vectors |
| **Euler Angles ↔ Quaternion** | HIGH | 1.5 hrs | Serialization, editor integration |
| **Quaternion from Rotation Matrix** | HIGH | 1 hr | `Quaternion(const Xformf&)` implementation |
| **Quaternion Lerp** | MEDIUM | 30 min | Linear interpolation (simpler than Slerp) |
| **Geometric Primitives** | MEDIUM | 2 hrs | Plane, Ray, AABB structs + utilities |
| **ConstBuffer Integration** | MEDIUM | 1 hr | Link projection matrices to renderers |

**Estimated remaining time**: ~6 hours (60% of original plan)

## Remaining Bugs & Robustness Issues

- [ ] Quaternion from matrix: needs trace method for stable conversion
- [ ] Slerp robustness: add shortest-path safeguard (negate if dot < 0)
- [ ] Vector ops: missing reflection, rejection methods

## Implementation Checklist

### Phase 2: Projection & View Matrices
- [X] `Mat4f Perspective(fov_degrees, aspect, near, far)` in Matrix.h
- [X] `Mat4f Orthographic(left, right, bottom, top, near, far)` in Matrix.h
- [X] `Xformf LookAt(Vec3f eye, Vec3f center, Vec3f up)` in Matrix.h
- [X] Vector utility: `T distance(Vec3f, Vec3f)` — Euclidean distance
- [X] Vector utility: `T angle(Vec3f, Vec3f)` — angle in radians [0, π]
- [ ] Tests for projection matrices (frustum validation)
- [ ] Tests for LookAt (camera pointing checks)

### Phase 3: Quaternion Enhancements
- [ ] `Quaternion(const Xformf&)` constructor — from rotation matrix via trace
- [ ] `Quaternion::FromEuler(pitch, yaw, roll, order)` — Euler to quat
- [ ] `EulerAngles Quaternion::ToEuler()` — quat to Euler, gimbal lock handling
- [ ] `Quaternion::Lerp(Quaternion, float)` — linear interpolation + normalize
- [ ] Slerp robustness: shortest-path correction, input normalization
- [ ] Tests: Euler round-trip (error < 0.001 rad), Lerp/Slerp correctness

### Phase 4: Geometric Primitives
- [X] `struct Plane` — normal, distance; point distance & ray intersection
- [X] `struct Ray` — origin, direction; point at distance, plane intersection
- [X] `struct AABB` — min/max; contains_point, aabb-aabb & ray intersections
- [ ] Utility: `Vec3f closest_point_on_segment(Vec3f p, Vec3f a, Vec3f b)`
- [ ] Utility: `Vec3f triangle_normal(Tri3d)` — via cross product
- [ ] Tests: plane/ray/aabb operations

### Phase 5: Integration & Polish
- [ ] Update ConstBuffer to use Perspective() and LookAt()
- [ ] D3D12App/VKApp: replace hardcoded transforms with builders
- [ ] Document matrix layout (row-major vs column-major for D3D12/Vulkan)
- [ ] Validate HLSL constant buffer layout matches C++

## Risk Assessment

**Low Risk**:
- Projection matrix builders (standard formulas)
- LookAt (well-known algorithm)
- Vector distance/angle (simple math)

**Medium Risk**:
- Euler angle conversions (gimbal lock edge cases)
- Quaternion from matrix (multiple stable formulations)
- AABB/Ray intersection (edge cases)

**High Risk**:
- Column-major vs row-major confusion (D3D12/Vulkan layout)
- Numerical stability of inverse/determinant at extremes

## Development Notes

- All operations should remain constexpr where possible
- Use existing patterns: `index_sequence` for vectorization, union for flexible access
- Maintain doctest for all new additions
- Matrix operations are row-major internally; document for shader integration

## Current Idioms to Follow

| Pattern | Example | Use In |
|---------|---------|--------|
| Constexpr index_sequence | `Seq_Data`, `Seq_Row`, `Seq_Col` | All vectorized operations |
| Union for access | `vec[i]` for rows, `data[i][j]` for 2D | All matrix types |
| Type aliases | `Vec3f = Vec<float, 3>` | New vector utilities |
| Inline implementations | `Mag()` in header | Simple functions |
| Static helper methods | `Slerp()` in namespace | Complex operations (not class methods) |
| Requires clauses | `requires(is_same_v<...>)` | Specialize for Vec3f (cross product) |

## Files to Modify/Create

```
src/math/
├── inc/
│   ├── Matrix.h             (ADD: distance, angle, projection builders, LookAt)
│   ├── Quaternion.h         (ADD: FromEuler, ToEuler, Lerp, FromRotMatrix)
│   └── GeometricPrimitives.h (ADD: Plane, Ray, AABB, utility functions)
├── Matrix.cpp               (ADD: implementations if needed)
├── Quaternion.cpp           (ADD: FromEuler, ToEuler, improved Slerp, FromRotMatrix)
├── Geometry.cpp             (NEW: Plane, Ray, AABB implementations)
└── test/
    └── MathTests.cpp        (ADD: projection, LookAt, Euler, primitive tests)
```

## Integration Points

1. **ConstBuffer** (`src/D3D12App/inc/ConstBuffer.h`)
   - Update `projection` matrix type when perspective() is ready
   - Add helpers for buffer creation from cameras

2. **D3D12App/VKApp** (`src/D3D12App/D3D12App.cpp`, `src/VKApp/VKApp.cpp`)
   - Replace hardcoded transforms with LookAt() and perspective()
   - Validate column-major expectations vs our row-major matrices

3. **Shader Constants** (`src/shaders/inc/`)
   - Document matrix layout (row vs. column major)
   - Ensure HLSL CB matches C++ layout

## Metrics to Validate

- [ ] All constexpr operations compile at compile-time
- [ ] No regression in existing tests
- [ ] New tests >90% branch coverage for Phase 2-4 additions
- [ ] Inverse accuracy: `|M · M^-1 - I| < 1e-5` element-wise
- [ ] LookAt produces camera frustums visible in scene
- [ ] Euler → Quat → Euler error < 0.001 radians
