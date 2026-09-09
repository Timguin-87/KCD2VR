# KCD2VR V58.9 — Mono Archaeology + Inventory Comfort + Conventional Sticks

This source candidate keeps the successful V58.7 world-fixed startup F6 quad and turns F8 into a focused 60-second mono-projection comparison.

## What V58.8 proved
- Startup/main-menu F6 world quad is the right presentation primitive.
- Double CStandardGraphicsPipeline::Execute really runs, but the final PRESENT path collapses the eye outputs and produced shadow smear rather than useful stereo.
- The old/current crop/pose changes did not create a meaningful visual difference in the headset.

## F8 — four 15-second mono phases
1. `MONO_A_CURRENT_CRYSIS`
   - current per-eye tangent remap
   - no global coverage clamp
   - honest common mono origin
2. `MONO_B_PRE46_BLACK_110`
   - approximation of the immediately pre-V46.6 mapping
   - manual 110° source VFOV, 0.95 framing
   - no global no-black clamp
   - uncovered rays are allowed to become black
   - old fake-separated projection pose retained for archaeology
3. `MONO_C_PRE46_BLACK_115`
   - same as B at 115° source VFOV
   - V46.6 documentation identifies 115° / 0.95 as its default when the no-black clamp was introduced, so this is the closest surviving-source reconstruction of the remembered lower black edge
4. `MONO_D_CYBERPUNK_CYCLOPEAN_DIRECT`
   - inspired by the uploaded CyberpunkVR package's explicit mono-submit / cyclopean-pose architecture
   - BOTH eye textures receive the same aspect-fill center copy
   - no per-eye tangent warp is baked into the pixels
   - common mono origin; OpenXR still owns projection-layer pose/FOV metadata

The Cyberpunk package is binary-only for the stereo plugin, so phase D is an architectural diagnostic, not a claim that its private implementation has been reproduced exactly.

## Inventory comfort
Short Left Menu now does two synchronized actions:
- sends vanilla `I`
- toggles the same LOCAL-space world-fixed game quad used for startup F6

This is based on the runtime observation that opening inventory causes KCD2's published camera VFOV to collapse to about 31°, which is unsuitable for the immersive mono remap. No global FOV heuristic is reintroduced.

## V58.9 controls
- Left stick: forward/back + strafe left/right
- Left stick click: sprint
- Right stick X: smooth horizontal camera turn
- Right stick click: crouch
- Left Y: jump
- Right A: interact/select/dialogue
- Right B: horse/companion context; hold dog call
- Right trigger: attack; while attacking, right stick selects attack direction
- Left grip: block
- Left trigger + left stick: torch / visor / quick weapon / quick consumable layer
- Left X: MIS menu
- Left Menu short: inventory + comfort quad toggle
- Left Menu hold: pause/Escape
- Right grip: still reserved for future physical grabbing/holsters

## Test request
For F8, do not judge black border as a feature. It is only a diagnostic marker. Please report which phase has the best *world scale / least binocular-wrap*, and whether B/C recreate the older better-feeling geometry even though an edge is uncovered.
