# Setup window and graphics options

## the setup window

the **KCD2VR - Setup** window opens by itself on the desktop at the first game start (a new install, no
`first_start=0` in KCD2VR.ini). no .cmd needed for that. it is on top of the game window and flashes in the taskbar.

- **Save for next start** writes your choices and sets `first_start=0`, so it doesn't open again. the settings are
  used from the next start (they wait in KCD2VR.next-start.ini until then), the running renderer is not changed.
  **Keep settings** keeps what is in the INI, **Skip this time** closes it.
- closing it without saving = it is offered again at the next start.
- to see it again at game start: set `first_start=1` in KCD2VR.ini (or delete the line).

you can also open it any time without the game: `Start KCD2VR.cmd` next to the dll. it is one line:

```bat
"%SystemRoot%\System32\rundll32.exe" "%~dp0dinput8.dll",KCD2VRSetupW
```

so you can put the same line in a config.bat if you like that better. from there **Save & start** saves and starts
the game through Steam, **Keep settings & start** starts it without your edits, **Cancel** starts nothing.

the game's -devmode start option is not needed. the cvars the mod sets that the game marks as cheat cvars are
written directly (R143).

## what you can set

- **Headset**: Quest 1/2/3/3S/Pro, PlayStation VR2, Valve Steam Frame, Other / custom. sets the source resolution;
  custom uses the width / height below.
- **Game + VR FOV**, **Immersive zoom**, **Theater / flat-screen scale**, **OpenXR render scale (0.6 - 1.3x)**.
- **Game graphics preset**: the game's complete Low .. Ultra preset. it unticks the VR performance overrides, you can
  tick them again. "keep current / custom" keeps your graphics preset.
- **VR performance overrides (distance, blur, sharpening)**.
- **Upscaling / antialiasing** and **Upscaling quality** (below).
- **Controller layout** (Quest Touch / OpenXR native and the others, `VrControllerLayoutR118`).
- **Developer mode - detailed VR menu and F-key diagnostics** (`VrDeveloperMode`): shows the Experiments pages in the
  VR menu and enables the recorder keys (DIAGNOSTICS.md). it is read once at game start. without it recenter,
  calibration, the VR menu (F7) and the F6 Theater/Immersive switch still work.

your calibration and the other settings in KCD2VR.ini are kept.

## upscaling

choices for stereo: **Native (no upscaling)**, **NVIDIA DLSS**, **AMD FSR**, **Intel XeSS**. the vendor modes run
one context per eye with separate histories. the game still decides on your GPU what is available. Native is the safe
choice.

Native writes an active literal line into the game-root `user.cfg`:

```ini
r_SuperResolution_Mode = 0
```

modes are 0 Native, 1 FSR, 2 DLSS, 4 XeSS. only **Keep current supported setting** (-1) leaves the line alone, and it
is offered only when user.cfg already has a mode the setup knows. the save reads the staged and the final file back
and reports a failure if the mode is not there, duplicate lines are removed. DLAA and the vendors' Native AA still
use their temporal backend, that is not the same as Native = 0. the DLSS quality mapping is 1 Quality, 2 Balanced,
5 DLAA.

the setup also writes `e_GsmSizesWH = 6,20,50,150,600` (the VR shadow cascades) into user.cfg (R145).

## changing graphics in the running game

the VR Graphics page in the VR menu switches live: it saves the request, stops the old stereo run, waits for it to
drain and changes the native settings, then stereo starts again. it says success only after a fresh stereo picture for
the new setting came out. if the setting applied but no matching picture comes within 8 seconds you see:

> SETTING APPLIED: STEREO OUTPUT UNCONFIRMED

the Apply button says **SAVED / REQUESTED** while it is pending. view modes are MONO and STEREO only.
