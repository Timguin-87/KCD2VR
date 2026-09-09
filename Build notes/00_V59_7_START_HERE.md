# KCD2VR V59.7 — Live Calibration + Corrected F6 Aspect

Recommended starting game config:

```ini
r_Width = 1920
r_Height = 1800
sys_MaxFPS = 72
cl_fov = 95
```

## New VR menu controls
- **World scale / head translation**: 0.80–1.20 in 0.01 steps. Changes positional head translation only; it does not post-warp the image.
- **Gameplay framing / zoom**: constrained to 0.95–1.20 because headset testing found values below 0.95 visibly warped.
- **KCD2 cl_fov**: 70–110. Move the slider, then click **APPLY**. The mod writes the native CryEngine `cl_fov` CVar once and reads it back; no refresh pulse is used.
- **KCD2 sys_MaxFPS**: 60–120. Move the slider, then click **APPLY**.

## Menu pointer
- Physical mouse maps directly to the F7 XR book canvas.
- Left Touch stick continuously moves the visible software cursor.
- Right trigger or A clicks. B closes the book.

## F6 aspect correction
F6 no longer assumes a 16:9 source. The flat-game quad uses the currently captured KCD2 source aspect (for example 1920x1800), so near-square game resolutions should no longer look vertically squashed.

## FOV policy
V59.7 accepts the game's current `cl_fov`, including values below 110. The old automatic `cl_fov` pulse is disabled. Runtime FOV changes happen only when you explicitly press the menu **APPLY** button.

Build marker:
`LIVE-CALIBRATION-20260905-A`
