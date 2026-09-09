# Native selected-camera-pair EXPERIMENT V10

Static discovery after V9:
- `WHGame+0x865550` computes `index = (CRenderView+0x8EE0) & 1`.
- It passes `RDX = CRenderView+0x8EE8 + index*0x310`.
- It passes `R8  = CRenderView+0x9508 + index*0x310`.
- It passes `R9  = [CRenderView+0xA720]`.
- It passes `RCX = CRenderView+0x9B30` as an embedded output state to
  `WHGame+0x866F6C`.
- `0x866F6C` immediately calls the known camera/frustum recompute routine
  `0x5E9890` on RDX, then builds a derived per-view camera/render state.

This means V7/V8/V9 only touching the base +0x8EE8/+0x9508 slots was incomplete:
there are indexed camera entries and the active pair is already resolved at
the `0x866F6C` boundary.

V10:
- keeps the V6 correlation/selection mechanism.
- hooks `WHGame+0x866F6C`, not the broad renderer orchestrators.
- recovers owner as `out_state - 0x9B30`.
- acts only when owner is the one correlated main-4E6C mode-2 CRenderView.
- translates exactly the selected RDX/R8 camera pair +0.032 local-right.
- calls the original pair builder.
- restores only the three translation floats on each selected camera.
- does not copy 0x310 blocks and therefore preserves the child's special state.
- no custom CRenderView allocation; no AER.
- `0x4E6924` and `0x4E7E64` remain disabled.

Key marker:
`NATIVE-PAIR-V10 apply=`
Important fields:
`offA`, `offB`, `beforeA`, `beforeB`, `context`.

Expected offsets should identify which indexed slots are active:
- bank A: 0x8EE8 or 0x91F8
- bank B: 0x9508 or 0x9818
