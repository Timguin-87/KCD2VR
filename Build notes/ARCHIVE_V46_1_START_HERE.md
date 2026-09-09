# KCD2VR V46.1 — Start Here

This is the current one-launch diagnostic package.

## What changed from V46

V46.1 keeps the corrected Gemini/Claude synchronization and eye-routing rules, then adds direct CryEngine runtime CVar tests for:

- `r_CustomResWidth`
- `r_CustomResHeight`
- `r_CustomResPreview`
- `r_CustomResMaxSize` (read/limit only)
- `cl_fov`

No `autoexec.cfg`, `user.cfg`, or keyboard console command is needed. V46.1 calls KCD2's own `IConsole::GetCVar` / `ICVar::Set` interface recovered from WHGame.dll.

It also measures whether the game actually:

- publishes a changed CCamera FOV;
- creates an exact-size CustomRes D3D12 texture;
- renders into that texture;
- reaches a type-1 Recursive View secondary render.

## Run

1. Build with the normal Windows/MSVC + DetourModKit + OpenXR setup.
2. Use the included KCD2VR.ini.
3. Load gameplay; visible water/reflections is preferred.
4. Press **F8 once**.
5. Let all **10 phases × 5 seconds = 50 seconds** complete.
6. Send `KCD2VR-debug...log` after `V46.1-MATRIX-COMPLETE` appears.

The mod automatically restores the original CryEngine CustomRes / `cl_fov` values and returns to mono Fit, swap off.

## Phase order

1. Baseline Fit
2. Baseline Stretch
3. Baseline Crop
4. CustomRes 1248×1344
5. CustomRes 1248×1344 + requested game FOV 110°
6. CustomRes 1872×2016 + requested game FOV 110°
7. Same + OpenXR FOV compositor remap
8. Canonical eye-route marker
9. Final-layer swapped eye-route marker
10. Recursive View observer

See `V46_1_CUSTOMRES_FOV_MATRIX.md` for interpretation and `V46_GEMINI_CLAUDE_CORRECTIONS.md` for why the external proposals were modified rather than copied literally.
