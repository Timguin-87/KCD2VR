# Stereo continuation after V61.19

V61.19 produced enough runtime evidence to stop hot-path logging.

Observed in one contiguous graphics-pipeline sequence:

- `CStandardGraphicsPipeline` dispatcher used Normal CRenderViews alternating between two stable addresses.
- The VolCloud stereo path reported its compiled `LeftEye` anchor on those same dispatches.
- The old CryEngine stereo backing values (`r_StereoStrength`, `r_StereoEyeDist`, `r_stereoScaleCoefficient`, etc.) were live at those dispatches.
- The CSystem slot-F8 candidate was called extremely frequently and returned false in the captured samples. Its call volume was high enough to make continued runtime logging inappropriate for VR timing.

Interpretation: the two alternating CRenderViews are now the best static lineage target. They are not yet proof of left/right scene eyes; they may be ordinary frame-buffered/scene-view objects. The next static work should compare their camera/projection/pass fields and trace the `LeftEye` registration/store sites into the pipeline before any state is forced.

V61.20 therefore installs no new stereo write and no new hot pipeline/gate detour.
