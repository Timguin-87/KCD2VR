# V60.0 — Tangent Zoom + Cyclopean Menu

Build ID: `TANGENT-ZOOM-CYCLO-MENU-20260905-D`

Changes from V59.9:
- Adds persistent VR-menu buttons: **MONO** (`fovAsym=0.00`), **PSEUDO 1** (`0.75`, V59.8 phase 4), **PSEUDO 2** (`1.00`, V59.8 phase 5/current normal projection).
- The selected mode is live and saved as `VrCyclopeanMode` (0/1/2).
- Extends the existing OpenXR-FOV/tangent framing slider down to **0.85** in 0.01 steps. This is still a uniform tangent-space scale of the perspective sample, not a Brown-Conrady lens warp. Going below source coverage can expose black/unavailable edges; it cannot invent missing angular content.
- Keeps the game/source FOV synchronization from V59.8.
- Keeps F8 all-in-one diagnostics from V59.9.

Optical note:
OpenXR owns headset lens distortion. The application should submit correctly projected per-eye images using runtime `XrFovf`; adding a second radial lens correction in the mod would normally double-distort the image.
