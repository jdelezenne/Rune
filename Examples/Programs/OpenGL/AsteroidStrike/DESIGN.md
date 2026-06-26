# Asteroid Strike

## Experience

Asteroid Strike is a fast, readable third-person space shooter. The ship must
feel attached to the controls: it banks while turning, accelerates visibly,
and occupies the lower center of the frame. The camera follows with a short
spring lag so rotation and thrust are visible instead of making the ship look
static.

## Spatial contract

- Ship-local `+X` is forward, `+Y` is up, and `+Z` is right.
- Rendering, camera targeting, collision, reticle placement, and projectile
  velocity all derive from the same `ShipBasis`.
- Forward speed is scalar simulation state. World velocity is derived every
  frame as `ShipBasis.forward * speed`, so turning the nose turns the flight
  path instead of preserving an unrelated lateral drift vector.
- The two guns are at local `(3.15, -0.08, +/-0.72)`. Alternating shots use the
  transformed muzzle position and the exact forward vector shown by the ship.
- The camera looks along the ship forward vector from behind and above it.

## Visual language

- The fighter has a long nose, armored fuselage, swept wings, raised canopy,
  twin engine housings, vertical fins, and emissive exhaust wedges.
- Asteroids are low-poly asymmetric rocks generated from uneven latitude rings.
  Each variant has axial deformation, face-level color variation, and a broad
  crater-like indentation; no sphere or icosphere topology is used.
- A dense three-layer star volume surrounds the arena. Stars are rendered
  before solid geometry without depth writes, producing obvious parallax.
- Warm projectiles and exhaust contrast with cold hull and star colors.
- Laser ribbons fade from an opaque muzzle-facing head to a transparent tail.
  Engine exhaust and destruction debris use camera-facing soft particles with
  depth testing and disabled depth writes. The world is rendered into an HDR
  framebuffer; a half-resolution bright pass is blurred horizontally and
  vertically, then composited over the full-resolution world before the HUD.

## Feedback

- Firing, rock destruction, and ship destruction emit transient frame events.
  Procedural SDL audio voices consume those events without making simulation
  code depend on an audio device.
- Rock explosions mix hot fragments with darker debris. Ship explosions are
  larger and mix orange hull energy with cold-blue engine fragments.
- A fixed-capacity particle pool keeps both VM and LLVM allocation-free during
  gameplay and lets effects continue to decay after game over.

## Game loop

- Mouse or all four arrow keys steer (left/right yaw, up/down pitch). A/D banks
  the ship, with Q/E as aliases; W thrusts; S brakes; left mouse or Space fires;
  R restarts after game over.
- Large rocks split into two medium rocks, then two small rocks.
- The arena wraps. New waves enter outside the initial camera view.
- The window title remains fixed. An SDL_ttf screen-space HUD reports score,
  lives, wave, speed, controls, and game-over state; the world-space reticle
  communicates the actual firing direction.

## Implementation constraints

- Observational APIs use immutable references (`*T`). Mutable references
  (`&T`) are restricted to simulation and resource construction.
- Solid meshes are procedural and uploaded once. A bounded dynamic buffer is
  reserved for alpha-blended ribbons and particle billboards.
- VM and LLVM must consume the same sources and simulation rules.
