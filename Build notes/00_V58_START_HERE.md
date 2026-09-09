# V58.1 BUILD FIX

This is V58 with the reported MSVC compile errors fixed. See `V58_1_BUILD_FIX.md`.

# KCD2VR / Medieval Immersion Simulator V58

## Purpose
V58 is a source build for runtime testing of four areas at once while keeping all risky native skeleton writes disabled:

1. a two-page/hybrid MIS control book with curved OpenXR-cylinder presentation when supported;
2. native KCD2 dialogue-camera experiments (`wh_dlg_ForcePOV` / `wh_dlg_NoCameras`);
3. provisional Quest Touch gameplay bindings and primitive read-only arm-chain diagnostics;
4. live OpenXR eye-swapchain resolution rebuild plus bridge-side sharpening and projection telemetry.

## Important safety / architecture notes
- **This source package is not claimed to be true stereo.** The normal gameplay bridge still copies the KCD2 mono backbuffer into both XR eye swapchains.
- V58 does **not** live-change KCD2 desktop resolution. `APPLY XR RES` recreates only OpenXR swapchains on the XR worker thread.
- The VRIK arm chain is diagnostic only. It estimates shoulder/elbow/hand positions from HMD/controller poses and performs **zero native KCD2 joint/IK writes**.
- The old FOV-driven automatic F6 menu detector is retired because action cameras produced false positives during bow/jump/combat recordings.
- The MIS mono-coverage calibration no longer writes KCD2 `cl_fov`.

## First test order
1. Build/install as usual and start with mouse/keyboard if desired.
2. Open MIS with F7 (or left Touch X when Touch input is active).
3. Verify the book appears in HMD and on the monitor. If VirtualDesktopXR exposes `XR_KHR_composition_layer_cylinder`, the HMD book should be gently curved; otherwise it falls back to a quad.
4. Drag a slider for >0.5 s: the book should fade to ~20% opacity while held and return on release.
5. Test `Sharpening` from 0 to a clearly high value; V58 applies sharpening in the XR bridge shader.
6. Test XR render scale, then press `APPLY XR RES`. Expect a short XR interruption/blink; the desktop/game resolution should remain unchanged.
7. Test Dialogue Camera modes: `Native`, `Force POV`, `No Cameras`. Record which one prevents the dialogue camera/FOV collapse without breaking dialogue.
8. Optional controller test: enable the Touch profile and verify movement/buttons before attempting combat.
9. F9 toggles the primitive VRIK arm-chain observer. It only logs estimated chains; no native skeleton writes occur.

## Provisional Touch layout (V58.6 test)
- Left stick Y: forward/back
- Left stick X: continuous smooth horizontal turn
- Left stick click: run/sprint
- Right stick X: strafe left/right
- Right stick up: jump
- Right stick click: crouch
- Right trigger + right stick: temporary four-way attack direction while attacking
- Left Trigger held + **right stick**: quick-action/D-pad layer (up torch, down visor, left quick weapon, right quick consumable; keyboard fallbacks are provisional)
- Right Trigger: attack
- Left Grip: block / temporary bow block-zoom behavior through vanilla input
- Right A: interact/select
- Left Y: temporary weapon slot 1/2 swap
- Left Menu short: inventory; hold >=0.5 s: pause/Escape
- Right B short/hold: temporary horse/dog context
- Left X: toggle MIS menu
- **Right Grip: intentionally unbound/reserved for future holsters and physical-object interaction**

## What to send back after testing
Please send both `KCD2VR.log` and the debug log. Useful notes are:
- HMD menu: curved/flat, size, clipping, opacity preview behavior;
- monitor menu: visible/interactable or not;
- dialogue mode result and whether FOV/zoom changes disappear;
- XR resolution scale requested and resulting eye size from the log;
- whether sharpening is visibly effective;
- Touch bindings that work/do not work;
- any F9/VRIK-chain log output.
