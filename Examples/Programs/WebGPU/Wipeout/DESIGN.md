# Wipeout (WebGPU)

A closed-circuit anti-gravity racer in the classic Wipeout mould: hover craft,
weapon pads, boost strips, AI pack, and a neon industrial look — all
procedural. No image, mesh, or audio assets are shipped.

## Platform shell

Follows `Examples/Programs/WebGPU/Pulse.rune`:

- **SDL3** owns the window, keyboard/mouse events, and synthesized audio.
- **WebGPU** owns the GPU device, surface, pipelines, and present path.
- Surface is created from the Emscripten `#canvas` selector (wasm-first, same
  as Pulse). The SDL window keeps the playground input path alive.
- Frame loop is `Platform.Emscripten.setMainLoop`.

## Experience

- Third-person chase camera locked behind the player ship.
- Always-on thrust with air-brake and boost pads.
- Left/right steer with visible bank; pitch for crests and drops.
- Weapons fire forward; pickups grant a single shot type.
- Three authored circuits with distinct rhythm; lap counter and finish.

## Spatial contract

- World is right-handed. Track travel is along the path **forward**;
  **right** is lateral across the road; **up** is the track normal (banked).
- Each ship stores:
  - `distance` — arc length along the closed centerline (wraps at total length)
  - `lateral` — offset along track right (−halfWidth … +halfWidth)
  - `height` — clearance above the road surface
  - `yaw` / `pitch` / `bank` — local attitude relative to the track frame
- Simulation and rendering share `sampleTrack(distance)` for center, axes,
  width, and surface flags. Collision is lateral clamp + wall bounce against
  `halfWidth`, and soft height spring toward hover clearance.

## Track authoring (not freeform meshes)

Tracks are **authored as closed control-point paths**, then **baked** at load:

| Authored | Generated |
|---|---|
| Control points (position, bank, half-width, flags) | Dense `PathSample` ring |
| Pad / scenery markers | Road ribbon mesh + walls |
| Theme palette | Vertex colors, fog, clear color |
| | Collision half-widths, boost regions |

Path samples store position, orthonormal frame, cumulative distance, half-width,
and surface kind (`normal`, `boost`, `rough`). The road mesh is a closed
triangle strip (two vertices per sample); walls are extruded side quads.

No external assets: geometry and audio are built in code.

## Visual language

- PS1-adjacent: flat-to-soft vertex colors, no textures.
- Dark void clear color; track stripes in cyan / magenta / amber per theme.
- Ships are low-poly wedges with emissive engine faces.
- Soft distance fog in the fragment shader (not a post stack).
- Weapon shots are bright elongated quads; boost pads pulse in the road color.

## Controls

| Input | Action |
|---|---|
| A / Left, D / Right | Steer |
| W / Up, S / Down | Pitch |
| Left Shift / Z | Air brake |
| Space / mouse left | Fire weapon |
| 1 / 2 / 3 | Select circuit (from ready / finish) |
| R | Restart race |
| Escape | Quit |

## Race loop

1. **Ready** — grid spawn, countdown timer, ships locked.
2. **Racing** — thrust, weapons, AI, lap detection via distance wrap + sector.
3. **Finish** — player completed required laps or destroyed (respawn with delay).

Laps: crossing the start line after leaving a far sector increments the lap.
Default race length: 3 laps.

## AI

AI ships track a target lateral lane and a target speed slightly below or above
the player (rubber-band). They fire when a rival is roughly ahead in the same
corridor and cooldown allows. No pathfinding graph — they follow the same
centerline samples as the player.

## Audio

Procedural SDL streams (Pulse / AsteroidStrike style): short synthesized hits
for fire, boost, wall graze, countdown, and finish. No sample files.

## Module layout

| File | Responsibility |
|---|---|
| `Main.rune` | SDL + WebGPU bootstrap, input, frame loop (Pulse shell) |
| `Track.rune` | Control paths, bake samples/mesh, sample queries |
| `Geometry.rune` | Procedural ship / shot / pad marker meshes |
| `Race.rune` | Ships, physics, weapons, AI, race state |
| `Render.rune` | Pipelines, buffers, draw world |
| `Audio.rune` | One-shot procedural tones |

## Implementation constraints

- Fixed-capacity pools (ships, shots, particles) — no gameplay heap growth.
- Observational APIs prefer immutable refs; mutation stays in sim updates.
- All art is procedural; the manifest ships no `resources`.
- Same sources for the wasm playground path used by other WebGPU programs.
