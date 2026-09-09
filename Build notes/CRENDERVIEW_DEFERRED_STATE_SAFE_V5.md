# CRenderView Deferred-State SAFE-V5

SAFE-V5 fixes the timing error in SAFE-V4 without adding any engine hook.

Why V4 read `source38=null`:
`WHGame+0x16F78F4` returns to the wrapper at `WHGame+0x4E6F50`. The wrapper configures the returned CRenderView only *after* the checkout observer returns, including the known `CRenderView+0x38` assignment and state copies around `+0x8D40/+0x8D50`.

V5 therefore:
- queues each genuine mode-2 CRenderView when checkout returns;
- inspects the previous queued view at the next already-hooked manager checkout, after its wrapper has completed;
- also flushes a queued tail view at the already-proven CSystem ViewCamera publication hook;
- adds no renderer hot-path detours;
- still does no custom CRenderView allocation and no stereo forcing.

Look for:
- `CRV-DEFERRED queued`
- `CRV-DEFERRED post-config`

The post-config line is the important one. It reports:
- CRenderView pointer and mode
- checkout caller
- `+0x18`
- configured `+0x38` source pointer, vtable and RTTI
- copied state at `+0x8D40` and `+0x8D50`
- whether the snapshot was triggered by `next-checkout` or `camera-publish`
