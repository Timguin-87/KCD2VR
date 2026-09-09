# V13 local/static test report

## What was tested without launching KCD2

- Exact supplied `WHGame.dll` disassembly around `0x865550` and `0x866F6C`.
- Win64 ABI for the selected-camera-pair builder.
- Selected camera-slot index/stride math.
- V13 no-write diagnostic source path.
- Reversible camera-translation helper: 100,000 deterministic randomized cases.
- OpenXR initialization failure cleanup path.
- OpenXR stereo-view snapshot validity/invalidation logic.
- Diagnostic projection-layer validity gating.
- Future feature gates remain inert/config-only.

## Stereo findings

1. `0x865550` selects `index = (CRenderView+0x8EE0) & 1`.
2. Camera A is `CRenderView+0x8EE8 + index*0x310`.
3. Camera B is `CRenderView+0x9508 + index*0x310`.
4. The call at `0x8655FB` places two additional float arguments in stack argument slots.
5. `0x866F6C` actually reads both values (`[rbp+0x570]` and `[rbp+0x578]` after its prologue), proving the old four-argument hook prototype was incomplete.
6. This `0x865550` path sets its iteration count to one (`CRenderView+0xA330 = 1`), supporting `owner = out_state - 0x9B30` for this invocation.

## V13 changes

- Correct hook ABI: `(out, camA, camB, context, float arg5, float arg6)`.
- Explicit forwarding of both floats on every original call.
- `EnableNativeEyeWrite=false` now logs selector/index, `offA/offB`, camera A/B translations, context and both floats without writing camera memory.
- Added stereo-hook teardown if OpenXR initialization fails.
- Invalidates cached stereo views when `xrLocateViews`/view flags are invalid, preventing stale eye poses from being reused later.
- Diagnostic projection submission now requires the same valid-position/orientation flags as the stereo snapshot.
- No future gameplay/FRIK/UI/crafting feature was enabled.

## Camera-write helper fuzz test

Temporary +3.2 cm local-X shift followed by restore returned the exact original 12-float camera storage in **100,000/100,000** randomized cases. Maximum restoration error: `0.0`.

This validates our reversible helper bookkeeping only; it cannot prove live KCD2 rendering semantics.

## First runtime settings

```ini
DiagnosticRendering = false
EnableNativeEyeWrite = false
```

Expected useful markers:

```text
NATIVE-PAIR-V13 selected
NATIVE-PAIR-V13 write-disabled
```

Please capture `selector`, `selected_index`, `offA`, `offB`, `A`, `B`, `arg5`, and `arg6` from the log.

## V14 continuation

V13 runtime showed zero translations in the selected child's A/B banks while its source CRenderView carried the real published camera. V14 therefore logs the post-0x866F6C generated output and correlates it against that source. The V7 doubled-view result remains the visual anchor; V14 is intended to identify the derived state responsible for that visible behavior before any new write is enabled.
