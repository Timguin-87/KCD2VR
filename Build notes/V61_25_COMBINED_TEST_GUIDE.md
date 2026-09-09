# KCD2VR V61.25 combined test

Source-only package. Windows/MSVC build has **not** been verified in this environment.

## Preserved known-good behavior
- V61.12 gameplay-only culling pose sync; camera matrix restored; FOV untouched.
- GOOD MONO: same KCD2 image may feed both eyes, but OpenXR keeps per-eye poses/FOV.
- FOV defaults 95/95, 90 FPS target, dialogue ForcePOV=1/NoCameras=0, A->E interaction.
- Proven FlashUI periodic retry remains.

## FRIK: F9
V61.25 fixes V61.24.1's validation bug. Gate 2 now validates only the verified buffer-1 API:
- CPoseData vtable +0x50 -> WHGame+0x53D830 getter
- CPoseData vtable +0x70 -> WHGame+0x53F4D0 setter

Gate 1 writes are intentionally disabled because WHGame+0x53D7C4 returns +0x10 into the 28-byte record and is not a proven full QuatT getter.

F9 cycles OFF -> joint+0xC -> joint+0xE -> BOTH -> OFF. The proof is an obvious +/-8 cm local-X pulse after COperatorQueue::Execute.

Look for:
- `V61.25-LATE-POSE MATCH`
- `V61.25-LATE-POSE APPLY`
- or explicit `V61.25-LATE-POSE REJECT ... reason=...`

## Live HUD: F12
F12 toggles only `CombatCursorCross` visibility. It uses the most recently observed live HUD object, then mirrors WHGame's own calls:
- HUD vtable +0x238 -> find `CombatCursorCross`
- cursor vtable +0x108(bool) -> visibility

It does not call the FlashUI global itself, does not write the HUD XML/PAK, and does not require restart.

Look for:
- `V61.25-FLASHUI LIVE-CURSOR visible=false`
- `V61.25-FLASHUI LIVE-CURSOR visible=true`

F10/F11 remain the existing animation-recording hotkeys.

## Stereo
`EnableStereoPipelineOffsetProbe = false`. The +0x818/+0x828 runtime observer is retired from this test path. No new hot stereo detour is added.
