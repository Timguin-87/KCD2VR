# KCD2VR V55 — Rotation cadence/smoothing + stereo re-entry + action recorders

## F8 — 6 tests, 10 seconds each
Audible ID remains: N short beeps = test N. Distinct two-tone signal = complete.

1. `TRACK_RAW_CACHED` — V54/V53-style cached pose, no filtering.
2. `TRACK_HOLD_45HZ` — HMD rotation is deliberately sample-held at 45 Hz.
3. `TRACK_HOLD_60HZ` — HMD rotation is deliberately sample-held at 60 Hz.
4. `TRACK_SMOOTH_TAU_8MS` — time-based quaternion smoothing using shortest-path SLERP with an 8 ms exponential time constant.
5. `NATIVE_STEREO_DUAL_EXECUTE` — same-epoch dual graphics-pipeline execute with ±½ live OpenXR IPD.
6. `NATIVE_STEREO_DUAL_EXEC_SELECTOR` — same as 5 plus renderer camera-selector toggle.

FOV, desktop resolution and presentation framing are unchanged across the ladder. Tests 5/6 are still renderer diagnostics; until separate per-eye targets are proven they must not be called true stereo.

### What to report
For tests 1–4, concentrate on smooth yaw, quick left/right reversals, pitch, and a few slow head circles. Say which beep number is best/worst and whether improvement feels smoother versus merely delayed/rubbery.

## Action recording after F8
The recorder does not depend on F7.

- **F10** toggles `WEAPON_COMBAT` recording.
- **F11** toggles `PICKUP_OBJECT` recording.

A high beep = recorder started. A low beep = recorder stopped. Each sample logs controller poses, trigger/grip, common keyboard/mouse inputs, XR frame, PRESENT count and applied pose frame using `V55-RECORD` markers.

Suggested weapon capture: press F10, perform several horizontal/vertical swings, stab, block, attack while moving, then F10 to stop.
Suggested pickup capture: press F11, look at/pick up/manipulate several objects using the normal game input, then F11 to stop.

F9 still toggles the experimental OpenXR hand-target/VRIK bridge.

## F7 safety
V55 prevents the VR-output bridge from being disabled by clicking its control inside the VR menu. This avoids the V54 black-headset failure caused by accidentally applying `bridge=false` from F7.

## Build status
Source package only. The local environment does not contain the Windows DetourModKit/MSVC toolchain, so no successful Windows ASI compile is claimed.
