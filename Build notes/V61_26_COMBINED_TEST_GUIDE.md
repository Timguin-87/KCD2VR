# KCD2VR V61.26 combined test

## What changed

- **Resolution:** no source/desktop resolution override. 1920x1800 is the preferred known-good performance baseline. OpenXR eye swapchains remain runtime-managed.
- **FRIK:** V61.25 proved real buffer-1 writes at `COperatorQueue::Execute(after)` but they had no visible effect. Native writes at this stage are retired. F9 now only logs that the proof is retired; capture/correlation stays read-only while the final skeleton/skinning owner is reverse engineered.
- **UI:** the proven FlashUI cursor control remains on F12. HUD scale/X/Y continue to use the known-good generated HUD XML semantics. **New:** releasing any HUD scale/X/Y slider automatically writes the generated HUD mod; no separate WRITE click is required. Restart KCD2 after changing those sliders.
- **Stereo:** static-only. The `+0x818/+0x828` hot pipeline probe remains disabled.
- **Culling / GOOD MONO:** unchanged.

## HUD test

1. Open the VR menu.
2. Adjust HUD scale / HUD X / HUD Y.
3. Release the slider. Look for `V61.26-MENU HUD XML AUTO-WRITE ... OK`.
4. Exit KCD2 normally and restart. The generated HUD layout should use the staged values.
5. F12 still exercises the already-proven `CombatCursorCross` live control; it does not scale the whole HUD.

HUD rectangle semantics remain: `left=X`, `top=Y`, `width=1872*scale`, `height=2016*scale`.

## FRIK

Do not expect F9 movement in this build. The OperatorQueue write branch is intentionally retired because V61.25 proved the write executes without affecting the rendered arm. The next FRIK target is final skeleton/skinning ownership, not another CPoseData write variant.

## Build status

Source package only. Windows/MSVC compilation has not been verified in this environment.
