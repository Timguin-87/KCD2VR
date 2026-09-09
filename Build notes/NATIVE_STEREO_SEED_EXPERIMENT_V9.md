# Native Stereo Seed Experiment V9

## Why V9 exists

V7 selected one genuine managed `mode=2` child on the `main-4E6C` route and
perturbed its `WHGame+0x865550` camera-build path. The child camera blocks
logged zero translations, but the user observed a doubled desktop image.
That is strong evidence the mode-2 child contributes a distinct rendered
view even though its embedded camera state is not initially populated like
the source view.

V8 then targeted the correlated source CRenderView. Runtime logging confirmed:
- source `+0x8EE8` contains live world-space camera translation
- source `+0x9508` becomes populated as the double-buffered peer
- `WHGame+0x865550` repeatedly consumes this source CRenderView camera state

## V9 experiment

V9 goes back to the **managed mode-2 child**, but before `WHGame+0x865550`:
1. Back up the child's two complete 0x310 camera blocks.
2. Copy the source CRenderView's complete `+0x8EE8` and `+0x9508` blocks into
   the child.
3. Add +0.032 game units along camera-local right to both seeded blocks.
4. Call the original engine camera-build routine.
5. Restore the child camera blocks byte-for-byte after the call.

This uses engine-owned CRenderViews and engine-owned camera structures only.

## Safety / scope
- no custom CRenderView allocation
- no AER
- no OpenXR composition changes
- `WHGame+0x4E6924` remains disabled
- `WHGame+0x4E7E64` remains disabled
- one managed child only
- child state is restored after each build call

## Key log markers
- `NATIVE-STEREO-SEED-V9 selected`
- `NATIVE-STEREO-SEED-V9 apply=`
- `seeded=true`
- `CRV-DOWNSTREAM EXPERIMENT-V9`

Expected if this is the right bridge:
- the desktop may show an even clearer two-view separation/doubling
- the seeded child should log real world-space `cam0_seed` / `cam1_seed`
  coordinates instead of zeros

This still does not submit separate left/right OpenXR images. It is a
renderer-side proof that a genuine managed mode-2 CRenderView can be driven
from the main source camera with an independent eye position.
