# KCD2VR V62.0 — Head Sync + Native UI + Semantic VRIK Ownership

## Scope

V62.0 consolidates the currently proven/desired paths and deliberately stops carrying the failed UI experiments forward.

### Preserved
- V61.12 gameplay-CView culling synchronization.
- GOOD MONO/OpenXR presentation lineage.
- V60.1/V61.36 dialogue/inventory source-FOV guard: no automatic `cl_fov` pumping.
- F7 manual FPS cap.
- `Fix head sync`: OFF = pose source 0 (`CACHED_XR`), ON = pose source 2 (`SYNC_NOW`).
- Semantic VRIK ownership ladder at `WHGame+0x5FE2F0`.

### Removed as runtime requirements
- Dynamic HUD.
- Dynamic Detector.
- KCD2VR Lua UI companion.
- HUD.xml runtime viewport writer as a live-UI solution.
- `hud.gfx` replacement for the V62.0 UI test.
- Generic `MasterSwitch` root scaling and percentage-to-`SetSubtitleSize` experiment.

## Native UI path

V62.0 uses the existing guarded ASI console bridge to invoke CryEngine `UIAction.SetPos` / `UIAction.SetScale` directly on MovieClips already declared by current KCD2 `HUD.xml`:

- `Subtitles` -> `bc.subtitles`
- `DialogLeft` -> `cl`
- `DialogRight` -> `cr`
- `Hints` -> `br.actionHints`

The current retail HUD design-space transforms were recovered from `hud.gfx`:

- `DialogLeft`: `(0, 540)`
- `DialogRight`: `(1920, 540)`
- `Subtitles`: `(-480, -230)` inside `bc`
- `Hints`: `(-35, -120)` inside `br`

F7 -> `TUNE UI` exposes:
- subtitle scale + vertical offset;
- dialogue scale + symmetric inward offset;
- interaction-hint scale + inward offset + vertical offset;
- master enable;
- `RESET VANILLA`;
- `APPLY UI NOW`.

Commands are issued only on explicit apply / slider release, never every XR frame.

## VRIK test in V62.0

V62.0 is still an ownership proof, not the finished controller-driven arm solver.

F9:
1. OFF
2. READ — resolve exact semantic arm map from the `+0x5FE2F0` skinning owner, zero writes
3. FINAL HAND_R — small post-original semantic right-hand final-output displacement
4. PREBUILD HAND_R — small buffer-1 semantic right-hand `QuatT` displacement immediately before `+0x5FE2F0`, then immediate restore
5. OFF

Retired paths remain retired:
- no OperatorQueue write;
- no WeaponSolver joint C/E as rendered hand indices;
- no `+0x53D7C4` full-QuatT assumption;
- no high-frequency file trace in the skinning hook.

## How the reference VR mods are used

The ownership proof is KCD2-specific and uses only KCD2 retail `WHGame.dll` RVAs/semantics.

After one semantic hand is visibly proven, production tracked arms should combine:

### Crysis VR pattern
- actual skeleton-derived upper/forearm lengths;
- analytic two-bone shoulder -> elbow -> wrist solve;
- controller grip as wrist target;
- stable elbow pole;
- preserve controller wrist orientation.

### Cyberpunk VR pattern
- let native animation run first;
- convert controller target into character/model space;
- cache the local-player rig;
- take ownership late in the animation/skinning pipeline;
- rewrite only the tracked arm chain instead of replacing the game's locomotion/combat animation system.

KCD2's production insertion point will be whichever V62.0 ownership state visibly moves the semantic hand.
