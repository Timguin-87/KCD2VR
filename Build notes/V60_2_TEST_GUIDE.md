# V60.2 test guide

Build ID: `REAL-FOV-CAL-MONO-RESET-20260905-F`

## What changed
- MONO is restored as a hard reset: tangent presentation, asymmetry 1.0, UV binocular offset 0, per-eye offsets 0, stereo experiment off, swap off.
- P6/P7/P8 remain available as separate live pseudo-stereo modes.
- View-mode changes no longer silently overwrite the Zoom slider.
- FOV Apply now uses the V60.2 deferred one-shot calibration from the real published camera FOV instead of permanently trusting `cl_fov` as the rendered FOV.
- The old >110 `cl_fov` result is treated as capped: 115/120/125 read back as 110 on this build of KCD2.

## First test
1. Start in MONO. Verify it is single and not doubled.
2. Switch P7 or P8, then back to MONO. Verify `V60.2-MONO-RESET` appears and the double-image state is gone without restart.
3. Set Zoom to a known value (for example 0.95), switch among MONO/P6/P7/P8, and verify the Zoom value does not jump.
4. In normal gameplay, Apply FOV 95, then 105, then 110. Wait at least one second after each Apply. Look for `V60.2-FOV-REALCAL` and report `realPublishedVFOV`.

Do not judge 115/120/125 through `cl_fov`; KCD2 clamps them to 110 in the observed runtime.
