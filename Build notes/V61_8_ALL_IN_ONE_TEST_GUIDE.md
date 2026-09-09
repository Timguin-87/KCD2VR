# V61.8 ALL-IN-ONE TEST

This is a source-only Windows/MSVC package.

## What changed
- GOOD MONO retained.
- Pseudo stereo: PS1 raised from 0.75 to 0.80; PS2 1.00 remains the preferred baseline; PS6 0.875 remains only for comparison.
- Balanced VR graphics: `e_MergedMeshesViewDistRatio` is forced to at least **250** for the culling stress test; view/vegetation remain >=120.
- CULL-POSE-SYNC: in injection mode 1, the current XR pose is temporarily applied only around `UpdateFrustumPlanes`, then the camera 3x4 matrix is restored. No FOV widening.
- F6 theater calibration: hold **Right Grip** and use **Right Stick Y** for distance (0.8-3.2 m), **Right Stick X** for screen size/framing (0.60-3.00). This never writes game FOV.
- Native stereo RT probe remains passive but can be ignored; no HW watchpoints or CVar ladder.

## Test order
1. Start with `EnableVrBalancedGraphics=true`. Reproduce edge culling while turning your head. Note whether culling is materially improved by CULL-POSE-SYNC.
2. F8: compare PS1 0.80, PS2 1.00, PS6 0.875. PS2 is expected to be best for normal world distance.
3. Press F6. Hold Right Grip + Right Stick to tune theater distance/size until the whole UI and reticle are comfortable.
4. Resolution tests (set in KCD2 externally, restart between each):
   - 1248x1344 (0.5x Quest eye dimensions)
   - 1872x2016 (0.75x)
   - 2496x2688 (1.0x / OpenXR recommended per eye)
   Compare circle/reticle shape, visible screen edges, text clarity, and performance.
5. UI: `r_ui_scale` is retired as a useful test; it had no visible effect. The Scaleform HUD patch remains a separate follow-up because mixing a binary GFX rewrite into this renderer/culling test would make attribution harder.
6. Dialogue auto-theater is not claimed as solved in this package. The uploaded ADB mods prove `ADLG_Cameras`/dialogue fragments exist, but we still need a safe runtime state hook before automatic entry/exit is reliable. Manual F6 works for this test.

## Logs to send
Send the debug log after 30-60 seconds of culling reproduction and after cycling F8. Interesting lines: `V61.8-CULL-POSE-SYNC`, `V61.8-PSEUDO-SELECT`, `V61.8-THEATER-CAL`, `GRAPHICS-APPLY`.
