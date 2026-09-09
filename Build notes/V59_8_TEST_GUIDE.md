# V59.8 Test Guide

1. Build with `build_release.bat`; it deletes the old build tree and verifies marker `FOV-SYNC-CYCLO-20260905-B`.
2. Keep `r_Width=1920`, `r_Height=1800`.
3. Open MIS menu. Set FPS to 90 and APPLY.
4. Set FOV to 95 and APPLY. Visually confirm the view now changes in the expected direction.
5. Optionally test 90 / 95 / 100 / 105 / 110, one at a time. Do not adjust the separate source-FOV control during this calibration.
6. When you pick the best FOV, press F8 once. The 56-second cyclopean ladder runs automatically.
7. For each beep/phase, note: depth impression, eye strain, head-motion swim, apparent scale, edge distortion, and whether polarity feels reversed.
8. After the ladder, normal fixed-8ms mono state is restored.

Expected debug evidence after FOV APPLY:

```text
V59.8-FOV-SYNC GAME_FOV=95.000 SOURCE_FOV=95.000 MATCHED=true origin=menu-apply
```

Expected F8 phase evidence:

```text
V59.8-F8-BEGIN ... GAME_FOV=95.000 SOURCE_FOV=95.000 MATCHED=true ...
```

Important: the cyclopean ladder is a pseudo-stereo calibration from a single mono image. A positive result may be useful perceptually, but it is not proof of true left/right scene rendering.
