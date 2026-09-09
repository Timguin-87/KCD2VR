# KCD2VR / Medieval Immersion Simulator V58.3 — regression repair

**Skip V58.2. Test this build instead.**

V58.3 is based on the V58.1/V58.2 source line, but it is specifically aimed at the regressions observed in the first V58.1 headset test: the startup/game view was too zoomed, the saved KCD2 FOV only became effective after moving the in-game FOV slider, the F7 book could show a flickering flat/monitor copy behind the XR cylinder, and Touch actions were detected by OpenXR but did not produce useful gameplay input.

## What changed

### 1. Startup/game FOV repair
- Restores the MIS **Capture FOV** baseline/reset to **130 degrees** (V56.1 behavior), not 115.
- Restores the familiar F7 labels:
  - `Capture FOV (MIS projection; game FOV stays separate)`
  - `Gameplay framing / zoom`
- Restores Capture FOV slider range to **100–160 degrees**.
- KCD2's own `cl_fov` remains independent from the MIS projection calibration.
- After the first real `ViewCamera` publication, V58.3 reads the user's existing `cl_fov`, briefly pulses it by 1 degree, and immediately restores the exact original value. This intentionally reproduces the slider up/down workaround without changing the final user setting.
- Debug marker: `V58.3-FOV-REFRESH`.

### 2. F7 clipping/flicker isolation
- `MirrorMisMenuToDesktop = false` is now the default.
- The book is rendered only through the dedicated OpenXR cylinder/quad layer during this test.
- This removes the V58.1 hybrid path that could leave a flat book image in a desktop backbuffer and then recapture it behind the XR layer.
- Debug marker: `V58.3-MENU desktop mirror DISABLED; dedicated XR menu layer only`.

### 3. Correct MIS stick
- **LEFT Touch stick** = F7/MIS D-pad navigation.
- Right stick remains gameplay locomotion when the book is closed.
- Right A / Right Trigger = activate/select.
- Right B = close/back.
- Left X = toggle MIS.

### 4. Touch injection diagnostics
The first V58.1 test proved that OpenXR Touch actions are being read by the mod, so this build concentrates on the next layer:
- Keyboard emulation now prefers Win32 **scan-code** injection rather than only virtual-key injection.
- Key down/up/taps, mouse buttons and failures are logged with `V58.3-INJECT`.
- Raw controller state remains rate-limited under `V58.3-TOUCH-STATE`.
- **Right grip is still RESERVED and unmapped.**

This is still a diagnostic Win32 input backend, **not** the planned ViGEm/XInput backend. If Windows accepts the injection but KCD2 still ignores it, the next control build should move directly to ViGEm rather than adding more keyboard hacks.

## Test order

1. Build/install V58.3 exactly as you normally do.
2. Start KCD2 and **do not touch the in-game FOV slider**.
   - Judge intro, initial menu and loaded gameplay against the last good pre-V58 view.
   - Search the debug log for `V58.3-FOV-REFRESH`.
3. Open F7.
   - The HMD should show one dedicated XR book/cylinder, without a flashing flat monitor copy behind it.
   - The desktop monitor intentionally does **not** mirror the MIS book in this test.
   - Confirm `Capture FOV` and `Gameplay framing / zoom` are back.
4. In F7, use **LEFT stick** to navigate/adjust; right stick must not navigate the book. Test Right A / Trigger and Right B.
5. Close F7. Test right-stick locomotion, Right A, Right Trigger, Left Grip, Left-stick click.
6. Send both logs. The most useful prefixes are:
   - `V58.3-FOV-REFRESH`
   - `V58.3-MENU`
   - `V58.3-TOUCH-STATE`
   - `V58.3-INJECT`

For this pass, dialogue can wait until the FOV/menu/input regressions are isolated.

## Architecture/safety status
- World rendering is still the mono KCD2 backbuffer copied to both XR eyes; V58.3 does not claim true stereo.
- VRIK remains observer/read-only; no native joint writes were added.
- No new WHGame hooks or renderer re-entry experiments were added.
- `EnableNativeEyeWrite = false` remains the default.
