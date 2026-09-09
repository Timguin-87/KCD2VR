# Stereo V14 — double-view correlation probe

## Why this branch

The earlier V7 visible doubled desktop image is retained as the strongest visual evidence that the selected CRenderView family can create more than one camera contribution in a single engine render. V9 then made the image clear after seeding the derived child from the populated source camera. V13 showed that the selected child camera-bank translations at the exact 0x866F6C boundary are zero, while the source CRenderView +0x8EE8 contains the real published gameplay camera.

Therefore V14 does **not** enable another camera write. It follows the data produced by the exact pair builder and correlates it back to the populated source. The objective is to discover what state the double-view mechanism actually generates before deciding where left/right OpenXR eye poses belong.

## Runtime behavior

Keep:

```ini
DiagnosticRendering=false
EnableNativeEyeWrite=false
```

V14 calls the original `WHGame+0x866F6C` unchanged, then inspects the resulting `out_state` for the already-correlated main-4E6C mode-2 child.

It logs:
- source CRenderView and source camera (`source+0x8EE8`)
- live source-camera position
- exact 0x30-byte source-camera matrix match inside the first 0x600 bytes of `out_state`
- pointer matches to camA, camB, source camera and context
- first 12 qwords of the generated output state
- the proven stack floats arg5/arg6

No V14 observer write occurs when `EnableNativeEyeWrite=false`.

## Marker

`NATIVE-PAIR-V14 post`

Important fields: `sourcePos`, `matrixMatch`, `ptrA`, `ptrB`, `ptrSourceCam`, `q0..q11`.

A non-negative offset is a direct correlation. `-1` means no exact match in the scanned 0x600-byte window and we continue downstream rather than assuming failure.

## Relation to V7

Do not discard V7. If V14 shows which output structure carries/points to the camera contribution, the next controlled write should reproduce the V7 doubling at that *derived output* boundary, then separate the two contributions using the actual OpenXR left/right poses rather than applying the same +X shift to both inputs.
