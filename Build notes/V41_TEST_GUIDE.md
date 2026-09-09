# V42 test

1. Build KCD2VR.asi with MSVC as before and copy `KCD2VR.asi` + supplied `KCD2VR.ini` beside the game executable/ASI loader.
2. Start in mode 1 (FrameLocked-AFR).
3. Do not tune Fit/Crop first. Verify the log identity sequence.

Expected good sequence:
```
V42-FRAME-PUBLISH serial=1 eye=L
V42-PRESENT-TAG ... frame=1 eye=L
V42-PRESENT-TAG ... frame=1 eye=L   (extra PRESENT is allowed)
V42-FRAME-PUBLISH serial=2 eye=R
V42-PRESENT-TAG ... frame=2 eye=R
```
The critical invariant is that PRESENT never flips eye inside one frame serial.

Also inspect `V42-NATIVE-DISPATCH`. We want to know how many `CStandardGraphicsPipeline` dispatches occur for each frame serial and which CRenderView/count are active. That determines whether V42 can safely duplicate/re-enter a renderer stage for synchronized/native stereo.

Use Home once if the right-side menu is visible in the left eye. Report which Home state makes UI land in the physically correct eye. Do not use IPD to diagnose eye routing.
