# Starting and changing graphics — R93R4 Repair R1

The R93R4 Repair R1 source patch leaves the launcher and stereo/upscaler
implementation unchanged. The descriptions below retain their original
evidence boundaries. Current changes and open work are in
[the repair handoff](R93R4_REPAIR_R1.md).

Open **Start KCD2VR.cmd** to choose your headset, resolution and graphics preset.
**Save & start** writes those choices before launch. The startup window is a
next-start configuration writer; opening it does not change a running renderer.

## Available choices

The current source offers **Native (no upscaling)**, **NVIDIA DLSS**, **AMD FSR**
and **Intel XeSS** for stereo. Vendor modes are candidates with separate eye
histories. Their final driver/headset output and image quality are not certified
by host tests. Native remains available.

DLAA and the vendors' Native AA options still use their temporal backend. They
are different from selecting Native with `r_SuperResolution_Mode = 0`.
The current launcher maps DLSS quality 1 to Quality, 2 to Balanced, and 5 to DLAA.
The short supplied D85R5 run records a live quality CVar of 2; that excerpt does
not establish a Quality-mode test or the active SDK render input dimensions.

**Keep current supported setting** is offered only when the current game-root
`user.cfg` contains a recognized literal mode. The save/launch path checks it
again. Modes are 0 Native, 1 FSR, 2 DLSS and 4 XeSS. Unknown expressions are not
silently treated as Native. The game still decides availability on the GPU.

**Keep settings & start** ignores edits in the startup window. **Save & start**
writes the selected mode, quality and any explicit preset. Cancel launches
nothing. Existing calibration and unrelated settings are preserved.

## Native must write mode 0

Select **Native (no upscaling)** in the upscaling dropdown, then **Save & start**.
The game-root `user.cfg` must contain an active literal assignment:

```ini
r_SuperResolution_Mode = 0
```

Native is the value 0. Only **Keep current supported setting** uses -1 and
intentionally leaves the upscaler assignment alone. The saved mirror key
`StartupUpscaler` does not decide the active renderer; the launcher reads the
config and the chosen combo item when saving. A panel-resolution preset named
Native and the vendors' Native AA modes are separate controls.

The source already mapped the explicit Native selection to 0. R3 strengthens
the save contract: it checks stream close, reads the completed staging file,
requires all explicit selections and the requested mode to match, installs the
file, then reads the installed file again. Missing mode 0, a conflicting later
assignment or an unsuccessful replacement cannot report a successful save or
continue the launch. Duplicate active assignments are removed by the existing
config updater. Unrelated settings and the existing quality values are retained
when switching to Native; they do not activate a vendor without its mode.

These checks verify what the launcher saved. They do not prove which process
Steam subsequently started or prevent an unrelated later writer from changing
the file. The reported failed Native selection has not been reproduced in the
supplied recordings, and no cause is assigned to the user's button choice.
The `.cmd` itself dispatches the setup code in the installed mod binary; this
source patch needs to be integrated into that binary to change its behavior.

## Changing the running game

The VR Graphics page uses the existing live-switch controller. It saves the
request, stops the old stereo run, waits for that work to drain and changes the
native settings. Context readiness then permits stereo rendering to start.

D85R5R1 reports a successful stereo switch only after a fresh output publication
for the requested technology in the new current epoch. Settings read-back or
creating two vendor contexts alone is insufficient. If settings apply but no
matching output appears within eight seconds, it reports:

> SETTING APPLIED: STEREO OUTPUT UNCONFIRMED

The Apply button says **SAVED / REQUESTED** while this process is pending.
Mono reports only that the setting applied. The current source has MONO and
STEREO; the historical AFR experiment remains inactive. FSR and AFR are separate
concepts.

A timeout cannot bypass completed-eye capture. The old D85 first-RIGHT-writer
fallback is superseded. Native eye projection, IPD, OpenXR composition and
accepted Fresh Pose timing retain their existing owners. The new optional
Fresh Pose Uncapped changes native-frame token pacing separately from this
upscaler-switch controller. Repair R1 also acquires an independent hand family
before a game update for head-source modes 0, 1 and 2. Its head lookup is a
coordinate reference only; it never publishes the camera head. Mode 3 keeps
its existing unavailable status. Shift+F7 reports selected/effective mode, locate
target, publication and the reason a fresh acquisition was unavailable.

## Presets and developer mode

Graphics presets select the game's complete Low through Ultra preset and untick
VR performance overrides. You can select those overrides again. **Keep current /
custom** preserves the corresponding graphics preset.

Developer mode exposes experimental pages and diagnostic shortcuts without
starting a recording. It is a startup choice. Recenter, calibration, the ordinary
VR menu (F7), the F6 Theater/Immersive switch and accepted hand-motion paths remain
available without it. Shift+F7 (VRIK recorder) needs developer mode.

See [the current repair handoff](R93R4_REPAIR_R1.md) and
[known issues](KNOWN_ISSUES.md) for the evidence and outstanding runtime checks.
No Windows DLL or headset image was produced for this source patch.
