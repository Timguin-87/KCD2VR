# MIS V46.4 — Stereo Ladder

Purpose: build directly on the user-confirmed good mono geometry from old V46.3.2 F8 phase 7.

## Known-good mono baseline
- stereo mode 0 (mono)
- CryEngine `cl_fov=110`
- compositor source vertical FOV 110 degrees
- presentation mode 3: OpenXR asymmetric-FOV remap
- zoom 1.0
- final eye swap OFF
- CustomRes untouched

The test build forces this baseline at startup and restores it after F8. The original CryEngine CVars captured before MIS applies the baseline are restored on mod shutdown.

## F8: 8 x 5-second stereo ladder
1. GOOD_MONO_XRFOV_110
2. AFR_HALF_IPD_XRFOV_110
3. AFR_FULL_IPD_XRFOV_110
4. AFR_FULL_IPD_SWAPPED_XRFOV
5. AFR_1_5X_IPD_XRFOV_110
6. SEQUENTIAL_PROBE_XRFOV_110
7. NATIVE_PROBE_XRFOV_110
8. RECURSIVE_OBSERVER_XRFOV_110

Modes 2 and 3 are still mono compositor fallbacks. Their purpose here is renderer/Recursive correlation, not a claim of true same-frame stereo. AFR is the only distinct-eye presentation currently armed.

## Safety
- no CustomRes changes in F8
- no CRenderView construction
- no dangerous `WHGame+0x4E6924` / `WHGame+0x4E7E64`
- no ABI change to `WHGame+0x653940`
- no new D3D12 device
- all stereo test phases are 5 seconds and reversible
