# V42 Renderer-Consumed Camera Proof

V42 retires ViewCamera publication as the stereo eye clock. The publish hook remains correlation-only.

Mode 1 now:
1. Assigns L/R at `CStandardGraphicsPipeline::Execute` (`WHGame+0x57278C`).
2. Reads the exact consumed `CRenderView` from `pipeline+0x1978`.
3. Selects its active camera bank at `CRenderView+0x8EE8 + (selector&1)*0x310`.
4. Applies the configured physical OpenXR IPD offset directly to that embedded camera around the pipeline call.
5. Restores the camera immediately after the original pipeline returns.

Marker: `V42-RENDER-CAMERA`. It prints the eye, offset, selected camera, and before translation.

This is an active causality test, not yet native same-frame stereo. Modes 2/3 remain renderer probes until re-entry is proven safe.

The broad CView census is disabled by default in V42 for performance.
