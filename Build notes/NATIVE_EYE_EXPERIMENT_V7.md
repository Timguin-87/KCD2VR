# Native Eye Experiment V7

Purpose: first controlled native-eye perturbation using an engine-managed derived CRenderView.

Static basis:
- `WHGame+0x4E6F1C` derives managed mode-2 CRenderViews from a source CRenderView.
- V6 runtime correlation proved the source CRenderView carries the published ViewCamera matrix at `+0x8EE8`.
- `WHGame+0x865550` consumes the CRenderView double-buffered embedded camera blocks at `+0x8EE8` and `+0x9508`, passing them to `WHGame+0x866F6C`, which treats the input as a CCamera and recomputes camera/frustum state.

V7 behavior:
- retains the stable V6 observers.
- keeps `WHGame+0x4E6924` and `WHGame+0x4E7E64` disabled.
- adds one exact-signature one-argument detour at `WHGame+0x865550`.
- does nothing until V6-style correlation sees a `main-4E6C` mode-2 child whose genuine parent CRenderView matches the published ViewCamera matrix at `source+0x8EE8`.
- atomically selects only that one managed child pointer as the diagnostic target.
- while `0x865550` runs for that exact child, shifts both embedded CCamera translations +0.030 game units along camera-local X/right.
- immediately restores only the original translation values after the engine camera-build call returns.
- no custom CRenderView allocation.
- no AER.
- no OpenXR composition changes.

Markers:
- `NATIVE-EYE-V7 selected`
- `NATIVE-EYE-V7 apply=`
- `CRV-DOWNSTREAM EXPERIMENT-V7`

Expected visual diagnostic if the selected derived view contributes to the visible world:
- some geometry/pass should show a small lateral parallax/culling shift.
- the desktop/main CView camera should remain governed by the existing HMD camera hook.

This is an experiment build, not labelled SAFE, because it introduces a new renderer-side detour at a statically identified CRenderView camera-build function.
