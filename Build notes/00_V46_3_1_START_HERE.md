# Medieval Immersion Simulator V46.3.1 — Parchment Menu

This is V46.3 Balanced with a **visual-only parchment/manuscript menu revision**.

## First run

1. Build from a **fresh CMake build directory** on Windows/MSVC.
2. Keep the normal safe defaults.
3. Load gameplay.
4. Press **F7** to open the parchment-style MIS panel.
5. If the panel appears correctly, close it and press **F8** once for the existing 10×5-second matrix when ready.

## Safety

The parchment revision does not add an engine hook, D3D12 device, OpenXR path, CVar write, or stereo experiment. It only changes GDI colors/fonts/borders inside the existing menu texture.

See `V46_3_1_WHGAME_DLL_CRASH_AUDIT.md` for the static audit against the uploaded `WHGame.dll`.
