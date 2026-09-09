# V30 — Consumed-parent record differential + XR FOV probe

V29 proved that the two frequently consumed mode-0 `CRenderView` owners both reach `CSceneGBufferStage` and bind the same CryEngine `CTexture` G-buffer/depth resources. That makes them poor candidates for an already-existing left/right render-target pair; they may instead be alternating renderer/frame-state owners.

V30 stays read-only and answers the next question before any new stereo write:

1. Capture the first two distinct mode-0 owners seen at `WHGame+0x653940`.
2. Snapshot the first `0x400` bytes of each `CRenderView+0x9B30` consumed record.
3. When A/B are observed within 32 renderer-consumer calls, compare 16-byte blocks.
4. Log the matrix/projection neighborhood `+0x140..+0x260`, including the already-proven renderer-read `+0x190..+0x1C0` region.
5. Log the last-good OpenXR left/right `XrFovf` values beside each sampled pair.

Markers:
- `RECORD-DIFF-V30 selected-A / selected-B`
- `RECORD-DIFF-V30 pair=`
- `RECORD-DIFF-V30 block off=`
- `XR-FOV-V30`
- `PIPELINE-V30`

Interpretation:
- A/B nearly identical except temporal/pointer fields and sharing V29 resources => likely frame/pipeline alternates, not native stereo eyes.
- Stable view/projection differences between A/B correlated to OpenXR-like asymmetry => candidate native stereo path worth activating.
- No suitable pair => next priority is retained CryEngine stereo-framework activation (`g_stereoFrameworkEnable`, `r_StereoEyeDist`, etc.), not arbitrary D3D12 CBV surgery.

No camera/projection/culling writes are performed by V30.
