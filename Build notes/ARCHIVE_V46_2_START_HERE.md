# KCD2VR V46.2 — In-Game VR Control Menu

V46.2 keeps the complete V46.1 CustomRes/FOV/Recursive View matrix and adds a live control panel.

## Controls

- **F7** — open/close KCD2VR Control Center.
- **F8** — run the existing one-shot 10 × 5-second matrix test.
- **Insert** — legacy stereo-mode cycle fallback (ignored while the menu is open).
- **Home** — legacy eye-swap fallback (ignored while the menu is open).
- **Esc** — close the menu while it is open.

## What the menu changes live

- VR output bridge on/off.
- Head rotation and head position.
- Head pose scale.
- Diagnostic rendering.
- Presentation mode: Stretch / Fit / Crop / OpenXR-FOV remap.
- Presentation zoom.
- Source vertical FOV used by presentation mode 3.
- Final eye swap.
- Stereo experiment mode 0–3.
- AFR IPD scale.
- Safe Mono Fit reset.
- Start the exact same 50-second V46.1 matrix test.
- Save the current live settings back to `KCD2VR.ini`.

## Important architecture

The menu does **not** create a second D3D12 device and does not hook DXGI Present.

It renders through the game's already-captured D3D12 device/queue in this order:

```
KCD2 submits final scene command list
  -> PRESENT barrier is correlated to that submission
  -> V46.2 draws the menu into that PRESENT backbuffer
  -> backbuffer is restored to PRESENT
  -> existing XrManager bridge samples the same resource
  -> OpenXR submission
```

Therefore the panel is intended to appear both on the desktop and in the current mono HMD view. The OS hardware mouse cursor is not part of the D3D12 backbuffer, so V46.2 draws a small software cross into the menu texture for headset use.

## First test

1. Build with a freshly regenerated CMake build directory.
2. Load the game normally.
3. In gameplay, press **F7**.
4. Confirm the panel appears and the software pointer follows the mouse.
5. Change `Fit` to `Stretch`/`Crop` and back to `Fit`.
6. Toggle head position or eye swap once and confirm the menu value changes immediately.
7. Press **Safe Mono Fit**.
8. Press **Save settings** if desired.
9. Use **Run 50 s matrix** when ready for the V46.1 resolution/FOV/Recursive test. The menu automatically closes before the matrix starts so it cannot obscure the visual comparison.

Send the log if the menu does not render. Useful markers are `V46.2-MENU initialized`, `V46.2-MENU GPU renderer initialized`, and `V46.2-MENU-DRAW`.
