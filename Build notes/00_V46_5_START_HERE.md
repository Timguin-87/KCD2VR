# Medieval Immersion Simulator — V46.5 Input + Stereo Prep

This source build follows the 2026-09-04 V46.4 headset test.

## What the test proved
- The OpenXR bridge is real and stable enough to show KCD2 in the headset.
- AFR 0.5x was visually closer to useful stereo than 1.0x+, but temporal ghosting is unacceptable as a final architecture.
- The menu mouse path remained unreliable.
- The user wants FOV controlled by the menu slider, not hard-coded to 110 degrees.

## V46.5 changes
1. **Mouse input replaced**: Windows mouse messages are consumed but no longer used to activate UI. The menu polls `GetCursorPos` + `ScreenToClient` + client-to-backbuffer mapping and edge-detects `GetAsyncKeyState(VK_LBUTTON)`.
2. **Keyboard fallback**: Up/Down select, Left/Right adjust, Enter activates.
3. **Input actions run on the Mod control thread**, not inside the D3D12 ExecuteCommandLists render callback.
4. **FOV slider is authoritative**: it drives both CryEngine `cl_fov` and the OpenXR-FOV compositor source model. F8 preserves the selected FOV.
5. **F8 is now 6x5 seconds**: Mono current FOV; AFR 0.25x; AFR 0.50x; AFR 0.50x swapped; Sequential re-entry observer; Native re-entry observer. No CustomRes writes.
6. **AFR diagnostic repaired**: camera eye now alternates on the frame serial and AFR logs whether OpenXR-FOV remap is active. This is still diagnostic only; AFR is not the final stereo architecture.
7. **Same-frame preparation**: F8 logs Normal and Recursive pipeline counts for the observer phases. It does NOT claim that same-frame render-twice is armed yet.
8. **Menu look**: Augusta is requested by font family name, with vector illuminated-manuscript-inspired colored vine/flower edges. No KCD2 texture/font files are redistributed.

## Safety
The dangerous WHGame+0x4E6924 / +0x4E7E64 paths remain untouched. CustomRes is not used by F8. Modes 2/3 remain mono compositor fallbacks until a verified scene render re-entry is identified.
