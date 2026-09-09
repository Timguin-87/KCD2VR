# V50 implementation notes

- Removed whole-backbuffer quad behavior from F6/F7 path.
- VrMenu now updates its parchment texture without compositing it into KCD2's PRESENT resource.
- Added a dedicated 650x990 BGRA OpenXR swapchain for the MIS menu when the runtime supports BGRA8.
- `xrEndFrame` may submit two layers: gameplay projection + centered head-locked F7 menu quad.
- F7 quad defaults: view space, 1.35 m forward, 0.86 m width, aspect derived from 650x990 texture.
- F8 is now a three-phase duplicate-frame / rotation-latency audit, 10 s per phase.
- `r_DuplicateLastFrameWH` is restored after F8/cancel.
- Phase 3 enables dense `V50-ROTATION-PUBLISH` logs at CSystem PublishViewCamera.
- No live resolution changes are performed.
- No guessed downstream camera writes are performed.
