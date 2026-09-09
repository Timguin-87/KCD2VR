# V59.9 — All-in-One Stereo / Projection Ladder

Build ID: `ALL-IN-ONE-STEREO-20260905-C`

Purpose: one F8 run tests every currently safe presentation/stereo route without changing game/source FOV during the ladder.

The ladder does NOT claim true stereo. Phases J/K are renderer re-entry diagnostics only and keep PRESENT on the mono fallback. A true-stereo success requires two distinct eye scene targets, not merely two executions.

Locked during F8:
- GAME_FOV == SOURCE_FOV (re-read and synchronized before start)
- fixed 8 ms rotation filter
- common-center pose
- no rotation reprojection
- no bottom-edge warp
- no AFR PRESENT bridge

Phases (7 seconds each):
1. `A99_BASE_Z095` — tangent baseline, zoom 0.95
2. `B99_ZOOM_093` — zoom 0.93 only
3. `C99_ZOOM_091` — zoom 0.91 only
4. `D99_CYCLO_075` — V59.8 preferred cyclopean asymmetry 0.75
5. `E99_CYBER_DIRECT` — Cyberpunk-inspired direct common-origin presentation
6. `F99_CYBER_DIRECT_C075` — direct + asymmetry 0.75
7. `G99_TANGENT_UV_002` — corrected tangent + small opposite-eye UV offset
8. `H99_TANGENT_UV_004` — corrected tangent + medium opposite-eye UV offset
9. `I99_SCENE_RT_MONO` — scene-color RT observer/control
10. `J99_DUAL10_SELECTOR` — double-render diagnostic, 10 mm test separation + selector
11. `K99_DUAL30_SELECTOR` — double-render diagnostic, 30 mm test separation + selector

Interpret J/K only from logs plus visible output. True-stereo evidence requires distinct L/R scene resources. Right-eye shadow smearing is a failure, not stereo proof.
