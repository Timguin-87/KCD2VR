# V61.9 — Pseudo Stereo + FRIK Capture Workflow

## Baseline
- V61.8 culling-only pose sync remains enabled.
- Balanced graphics returns to 120 / 120 / 120 for view / vegetation / merged meshes.
- `sys_MaxFPS` is applied to 90 once when VR starts. The menu slider can still override it.

## Pseudo stereo
Retained choices: MONO, PSEUDO .80, PSEUDO .85, PSEUDO .95.
F8 cycles `.80 -> .85 -> .95 -> MONO`. `.95` is the preferred candidate. UV/AFR/DUAL modes are removed from the active workflow.

## Merged mesh performance test
The old native-stereo area on the right page is repurposed:
- slider: 30..250
- APPLY: commit current value
- ORIGINAL 30
- BALANCED 120

Now that culling orientation is fixed, find the lowest merged-mesh value that keeps visibility acceptable.

## FRIK recording
Starting any action recorder automatically enables the read-only VRIK arm/hand observer. `V61.9-FRIK-SAMPLE` logs controller pose + trigger/grip + estimated shoulder/elbow/hand/reach alongside the existing input/frame samples. No native skeleton writes.

Suggested 10-20 s captures: COMBAT, PICKUP, JUMP/CROUCH, ARCHERY, USE/EAT, HORSE, DIALOGUE, then ALCHEMY/SMITHING if convenient. Press the same category again to stop. High beep=start; low beep=stop.
