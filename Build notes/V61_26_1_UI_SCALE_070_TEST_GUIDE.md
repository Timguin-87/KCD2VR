# KCD2VR V61.26.1 — UI scale 0.70 proof

Purpose: isolate the HUD-size path before doing more FRIK/stereo work.

## What this test changes
- Forces/stages HUD XML scale = **0.70** at startup.
- X = 0, Y = 0.
- Original HUD canvas 1872x2016 becomes approximately **1310x1411**.
- The generated HUD remains anchored at top-left according to the proven V61.22 semantics.
- FRIK native write experiment remains retired/read-only.
- Stereo remains static-only.
- 1920x1800 source-resolution baseline is not changed.

## Test
1. Build/install this source as usual.
2. Launch KCD2 once and reach startup/gameplay long enough for KCD2VR to initialize.
3. Confirm the debug log contains:
   `V61.26.1-UI-SCALE-TEST staged scale=0.70 x=0 y=0 -> 1310x1411; RESTART GAME TO APPLY`
4. Exit KCD2 completely.
5. Relaunch.
6. Check whether the entire KCD2 HUD is clearly smaller.

The HUD PAK route requires restart. This test intentionally writes a fixed 0.70 value so stale INI values cannot mask the proof.
