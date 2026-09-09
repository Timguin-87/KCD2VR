# CRenderView Source Camera Control V8

V7 result:
- target selection worked.
- `WHGame+0x865550` hook fired repeatedly.
- selected mode-2 child had `(0,0,0)` translation in both `+0x8EE8` and `+0x9508`.
- therefore V7 perturbed a derived object whose embedded camera blocks were not populated with the visible ViewCamera.

V8 correction:
- selection criterion remains the same runtime proof: main-4E6C derived child whose genuine source CRenderView matches the published ViewCamera at source+0x8EE8.
- target changes from the derived child to that genuine source CRenderView.
- at `WHGame+0x865550`, apply +0.050 camera-local-right translation to source `+0x8EE8` and `+0x9508` only while the original camera-build function executes.
- restore translations immediately on return.
- no custom CRenderView allocation.
- DiagnosticRendering may remain false.
- 0x4E6924 and 0x4E7E64 stay disabled.

This is a CONTROL experiment, not the final stereo mechanism. Success criterion:
- log shows non-zero/world-space `cam0_before` values on `CRV-CAMERA-CONTROL-V8 apply=`.
- ideally a small but visible lateral shift/parallax/culling change appears in the normal world.

If successful, we have proved the exact engine camera-build path and can then redirect/copy a real source camera into a managed derived mode-2 view for the actual second-eye experiment.
