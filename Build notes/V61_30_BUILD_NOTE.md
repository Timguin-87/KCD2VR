# V61.30 build note

Purpose: one-launch all-hypotheses diagnostic branch built on V61.28's known-good combined baseline.

Hotkeys:
- F4: DOF-focus-driven pseudo stereo + lazy read-only Normal G-buffer depth-resource probe, ON/OFF.
- F6: exact-gameplay-CView actual FOV + XR-derived vertical optical-center correction, ON/OFF.
- F7: 20% smaller XR menu + restored right-Touch ray cursor.
- F8: passive source-sized StereoL/R producer candidate capture, ON/OFF.
- F9: OFF -> HAND final-skin pulse -> FULLBODY semantic BoneID-table pulse -> OFF.
- F10/F11 recording and F12 FlashUI cursor proof retained.

Safety/baseline:
- V61.12 gameplay-only cull synchronization retained and cull FOV untouched.
- 1920x1800 KCD2 source baseline is not overridden.
- OperatorQueue FRIK writes remain retired.
- FULLBODY F9 requires semantic HAND_L/R table entries to match live Henry joints before any body write.
- F4 depth work is read-only and lazy-installed; no GPU resource mutation.
- old F4 adaptive-rotation mode retired; fixed 8ms remains.
- Insert/Home AFR/native-eye controls remain inert.
- `EnableStereoPipelineOffsetProbe=false`, `EnableNativeEyeWrite=false`, `EnableLegacyStereoProbes=false`.

Source only; Windows/MSVC compilation is not verified in this environment.
