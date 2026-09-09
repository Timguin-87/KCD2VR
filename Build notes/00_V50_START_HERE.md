# KCD2VR V50 — Rotation Latency + Dedicated F7 Quad

## What changed

V50 deliberately stops changing game resolution, FOV, presentation framing, or FPS during the F8 audit.

### F7
F7 now keeps gameplay in the normal OpenXR projection layer and submits the MIS parchment as a second, head-locked `XrCompositionLayerQuad`.

The menu is sourced from its existing 650x990 GPU texture and copied to a dedicated OpenXR UI swapchain. The KCD2 backbuffer is not flattened or replaced by the menu quad.

Expected log markers:
- `V50-F7-QUAD UI swapchain ready 650x990 ...`
- `V50-MENU-TEXTURE ... backbuffer untouched`
- `V50-F7-QUAD ON additive head-locked menu-only layer ...`

F6 whole-view quad is retired in this build.

### F8: 30-second rotation-latency audit
All phases preserve full 6DoF and keep the same visual presentation.

1. 10 s — `BASELINE_DUPLICATE_ORIGINAL`
2. 10 s — `DUPLICATE_LAST_OFF`
3. 10 s — `DUP_OFF_DENSE_ROTATION_TRACE`

Phase 2/3 force `r_DuplicateLastFrameWH=0` and restore the original value after the audit. Phase 3 additionally logs every `PublishViewCamera` rotation matrix with the most recently consumed XR pose-frame identity and pose-apply age.

During F8 continuously perform yaw/pitch/roll turns, especially smooth slow yaw followed by quick reversals. Also lean/translate so rotational and positional behavior can be compared.

## Important architectural limit

V50 does not claim native stereo. The gameplay bridge still uses the captured KCD2 backbuffer as the scene source for both eye swapchains. The dedicated F7 quad fixes only the MIS menu composition architecture.

V50 also does not write a guessed downstream camera matrix. Earlier WHGame.dll probes prove downstream consumers exist but do not yet prove a safe writable rotation slot. Phase 3 is intentionally dense read-only evidence for choosing that write point.

## Build

Use the same Windows MSVC / DetourModKit / vcpkg environment as V48/V49. `build_release.bat` is retained.
