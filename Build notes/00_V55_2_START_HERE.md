# KCD2VR V55.2 — crash isolation + native menu flat mode

## Crash fix
The uploaded crash occurred immediately after the first tracked camera publish, before F8.
V55.2 restores default/raw tracking to the exact pre-filter path: filter mode 0 returns the normalized target immediately and never touches QPC/thread-local filter state.
The 45 Hz / 60 Hz / 8 ms experiments only activate after F8 enters those phases.

## Native KCD2 menus
Press **F6** when opening Inventory / Map / Quest / Codex / pause-style full-screen UI.
- one high-ish beep: native-menu flat mode ON
- one low beep: OFF / gameplay projection restored

Flat mode disables the OpenXR FOV remap for the KCD2 backbuffer and presents it aspect-correct at a smaller comfortable scale.
The head anchor is captured only once on entry (V55/V54 previously recaptured it every loop, which was a bug).

This is deliberately a safe manual switch for this build.  Exact automatic menu/overlay detection will be hooked separately once the KCD2Tools UI hook anchors are ported/validated against this retail WHGame.dll.

## Other controls
- F8: V55 tracking/stereo test ladder with beep count
- F9: experimental hand-target/VRIK bridge
- F10: record weapon/combat
- F11: record pickup/object interaction
- F7: KCD2VR parchment control menu
