# CRenderView Checkout-Only SAFE-V3

Purpose: isolate the proven engine-managed CRenderView lifecycle without detouring renderer hot-path functions.

Disabled:
- `WHGame+0x4E6924` renderer/world orchestrator
- `WHGame+0x4E7E64` render/pass object query

Retained:
- `WHGame+0x16F78F4` CRenderView manager checkout observer
- `WHGame+0x1DED8A0` generated CRenderView factory observer
- `WHGame+0xBBCFF0` CRenderView constructor observer
- previously stable CView/CSystem/C3DEngine camera observers

Expected log marker:
`CRV-DOWNSTREAM SAFE-V3: invasive renderer detours disabled (0x4E6924 and 0x4E7E64); using managed CRenderView checkout/factory/ctor observers only`

What to capture:
1. Load a save and play/move the camera for roughly 30-60 seconds.
2. Exit normally if possible.
3. Send the resulting KCD2VR-debug log.
4. If it crashes, also send BugSplat/minidump information.

This build performs no custom CRenderView allocation and does not force stereo.
