# V60.5 test guide

Press **F8 once** in normal gameplay. Do not touch the FOV, zoom, pseudo buttons, or culling slider during the 40-second ladder.

Each phase lasts 8 seconds and should beep/change automatically:

- Phase 1: 100.0°
- Phase 2: 102.5°
- Phase 3: 105.0°
- Phase 4: 107.5°
- Phase 5: 110.0°

Please note for each phase:
- black edges: yes/no
- apparent zoom
- straight-line/edge warping
- head-motion stability
- sharpness

The ideal result is the lowest FOV with no black edges and no new geometric warp.

Also test one switch from your preferred pseudo mode to **MONO** after F8 and confirm whether the double-view leak is gone.

Expected log markers: `V60.5-F8-START`, `V60.5-F8-BEGIN`, `staticH=YES`, `V60.5-F8-COMPLETE`.
