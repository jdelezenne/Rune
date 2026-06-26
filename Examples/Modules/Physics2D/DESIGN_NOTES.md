# Physics2D — design notes

Physics2D is a deliberately small, demonstration-grade 2D physics module
(circular rigid bodies, walls, pockets) built on the `Standard` math module. These
notes record how it leans on `Standard.Math`, what a "real" 2D physics module
would still need, and how a Physics3D counterpart would be shaped.

## Reliance on the Standard math module

The module defers all geometry to `Standard.Math` instead of re-deriving it:

- Walls are `Segment2D` and pockets are `Circle` directly — the old `Wall2D` and
  `Pocket2D` duplicate types have been removed (they were `Segment2D`/`Circle`
  under other names).
- Contact generation uses `Segment2D.closestPoint`, `Circle.contains`,
  `Vector2.cross`/`rotated`, and the shared dot/length/normalize helpers.
- All body/velocity/impulse math is `Vector2`.

New collision code should build on the math primitives directly:

- `Segment2D` — closest point/distance, segment crossing.
- `Circle`, `Rect`, `Polygon2D` — containment, area/centroid, SAT.
- `Intersection.circleCircle / circleRect / rectRect / polygonPolygon` — overlap.
- `Vector2.cross` (perp-dot), `rotated`, `clampLength`, `moveTowards` — the 2D
  dynamics vocabulary (torque sign, steering, speed capping).

## Implemented so far

- **Angular dynamics.** `RigidBody2D` carries `rotation`, `angularVelocity`, and
  `invInertia` (disk `½mr²`, box `m(hx²+hy²)/3`). The impulse solver
  (`resolveContact` / `resolveStaticContact`) applies a normal impulse with
  restitution and a Coulomb-clamped **friction** impulse, both including the
  angular arm (`r × impulse` via `Vector2.cross`), so glancing contacts impart
  spin and throw. Integration advances `rotation` and damps spin.
- **Shape variety.** Circle and box bodies, with contact generation for
  circle-circle, circle-box, box-box (SAT), circle-wall, and box-wall. `collide`
  dispatches by shape pair; `Collision2D` now carries a contact `point`.

## What is still missing (for the next dev)

- **Multi-point manifolds.** Box contacts use a single representative point,
  which is fine for impacts but jitters for resting stacks. Add reference/incident
  face clipping for two-point manifolds, and accumulate impulses over solver
  iterations (warm starting).
- **Iterative solver + position stabilization.** The current resolve is a single
  pass per contact; a sequential-impulse loop with Baumgarte/slop would be far
  more stable.
- **`Polygon2D` and capsule bodies.** `Polygon2D` (with SAT via
  `Intersection.polygonPolygon`) exists in the math module but is not yet wired in
  as a body shape.
- **Broadphase.** Collision is O(n²); add a spatial hash / sweep-and-prune over
  `Rect`/`Box` bounds.
- **Constraints/joints**, **continuous collision** (fast bodies can tunnel), and
  **forces/gravity** accumulators (only linear + spin damping today).

None of the above are blocked by the math module. The one math primitive a
polygon/capsule engine would still want is a **segment-vs-box/polygon distance
with closest features** (the deferred `capsule*Box` cases in `Intersection`).

## Physics3D — direction

A Physics3D module would mirror this structure and is unblocked on the math side:

- `RigidBody3D`: `position: Vector3`, `orientation: Quaternion`, linear +
  `angularVelocity: Vector3`, `mass`, and an inverse inertia tensor as a
  `Matrix3` (rotated into world space each step via `orientation.toMatrix3`).
- Colliders: `Sphere`, `Box`, `OrientedBox`, `Capsule` (all present), with
  overlap via `Intersection.*` and raycasts via `Intersection.ray*`.
- Integration uses `Vector3` + `Quaternion` (integrate orientation by the
  angular-velocity quaternion, then `normalized()`).
- Broadphase over `Box` (AABB) bounds; `Frustum` is available if the caller wants
  visibility culling.

Remaining math gaps for 3D specifically: **contact manifold generation** (normals,
penetration, contact points) for box/OBB/capsule pairs, and **inertia-tensor
builders** for the primitive shapes — both engine-level, layered on the existing
distance/closest-point queries rather than new core math.
