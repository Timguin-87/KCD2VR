# V46 - Gemini + Claude Corrections

This note records exactly what was accepted, rejected, or modified from the two external proposals and why.

## Gemini proposal

### Accepted: one OpenXR frame sample / immutable frame snapshot
Gemini's strongest idea is correct: `xrWaitFrame`, `xrBeginFrame`, pose/view location, camera consumption, and final composition should share one frame identity and one `predictedDisplayTime`.

V46 keeps OpenXR lifecycle ownership inside the existing `XrManager::RunFrame` worker instead of adding a second independent `FrameSync::BeginXRFrame` caller. `RunFrame` performs the single `xrLocateViews` call and writes a `RenderFrameState` containing:

- frame index
- predicted display time
- shouldRender
- sampled head pose
- both eye poses
- both eye FOVs
- view-state validity
- plausible IPD

The final OpenXR projection layer and the compositor FOV remap now read this exact cached state.

### Accepted and corrected: camera timing
Previously the worker returned from `RunFrame` (including `xrEndFrame`) and only then `Mod` called `CameraHook::SetHeadPose`. V46 publishes the pose sampled at `predictedDisplayTime` to `CameraHook` immediately after sampling/input sync and before the render/submission handoff.

This removes avoidable pose latency. It does **not** magically make the already-flat KCD2 backbuffer same-frame stereo; the future Recursive right-eye renderer must consume the same `RenderFrameState` for that.

### Rejected: Gemini's direct `CopyResource` path to XR eye textures
The current KCD2 source is typically 1920x1080 while the VDXR/OpenXR eye swapchain is 2496x2688. D3D12 `CopyResource` requires compatible resources with identical dimensions/size, so it cannot scale this source into the XR eye image. V46 retains the fullscreen shader blit.

### Corrected: OpenXR D3D12 swapchain resource state
Gemini's sample assumes XR color images start in `PIXEL_SHADER_RESOURCE`, transition to `COPY_DEST`, then return to `PIXEL_SHADER_RESOURCE`. That is wrong for `XR_KHR_D3D12_enable`.

For a waited D3D12 OpenXR **color** swapchain image, the runtime guarantees `D3D12_RESOURCE_STATE_RENDER_TARGET`, and the application must return/release it in `RENDER_TARGET` state.

V45 incorrectly transitioned XR eye images `COMMON -> RENDER_TARGET -> COMMON`. V46 removes those transitions. Only KCD2-owned source/cache resources are transitioned by us.

### Not solved by Gemini
Gemini's proposal synchronizes pose/FOV and copy/submission, but it never creates a second KCD2 scene render. It is therefore a timing/correctness improvement, not native stereo by itself.

---

## Claude scaffold

### Valid observation
Claude correctly noticed that `BlitAfrStereoToEyes` itself does not read `swap_eyes_`.

### Rejected patch: early AFR destination swap
Claude adds:

```cpp
if (swap_eyes_) {
    std::swap(dsts[0], dsts[1]);
}
```

But V45 already applies `swap_eyes_` later when building the final `XrCompositionLayerProjectionView` mapping:

```text
physical eye 0 <- swap ? XR texture 1 : XR texture 0
physical eye 1 <- swap ? XR texture 0 : XR texture 1
```

So Claude's patch would perform two swaps:

```text
early blit swap + existing final OpenXR swap = cancellation
```

V46 deliberately keeps canonical internal routing:

```text
L source/cache -> L XR texture
R source/cache -> R XR texture
```

and allows **exactly one swap**, at final OpenXR projection-layer routing.

### Added proof instead of guessing
V46 matrix phases 5 and 6 render unmistakable markers into the canonical eye textures:

- canonical L texture: red block at top-left
- canonical R texture: green block at top-right

Phase 5 uses final swap OFF. Phase 6 uses final swap ON. If the blocks physically exchange eyes, the final routing is proven.

### Claude's Recursive View direction
Claude's archive mostly reuses V45 here. The Recursive View direction remains valid and is retained: KCD2's engine-owned `EViewType::Recursive` render-to-texture path is observed, not manually constructed or invoked.

---

## Why V46 does not include AFR in the automatic matrix

V39/V41 AFR eye identity was never trustworthy enough to answer native-stereo questions. PRESENT transitions are not a game-frame boundary, and later camera-bank edits proved ineffective visually. Including AFR would make the matrix noisier without answering the current problem.

The automatic matrix therefore keeps `VrStereoExperiment=0` throughout. It tests:

1. the known-authoritative mono bridge,
2. all presentation mappings,
3. OpenXR FOV remap,
4. final eye routing exactly once,
5. the real CryEngine Recursive View/VRCAM candidate.

Native stereo remains the next implementation step **if** the Recursive View chain is proven live through `CStandardGraphicsPipeline` into a separate render target.
