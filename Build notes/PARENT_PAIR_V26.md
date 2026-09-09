# V26 dual consumed-parent pair causality

V24/V25 established that the selected mode-2 child is not consumed at WHGame+0x653940, while two mode-0 source parents are consumed in an almost 1:1 cadence. V25 moving only parent A by +100 mm caused the visible image to double.

V26 captures the first two distinct main-4E6C mode-0 source parents with a valid +0x8EE8 gameplay-camera match and controls both reversibly at WHGame+0x865550:

- 0-5 s: baseline, no writes.
- 5-10 s: parent A +100 mm, parent B -100 mm.
- 10-15 s: parent A -100 mm, parent B +100 mm.
- >=15 s: parent A = OpenXR left half-IPD, parent B = OpenXR right half-IPD.

Markers:
- PARENT-PAIR-V26 selected-A
- PARENT-PAIR-V26 selected-B
- PARENT-PAIR-V26 baseline
- PARENT-PAIR-V26 apply
- RENDER-CONSUME-V26

All camera translation writes are restored after the original broad camera-build call.
