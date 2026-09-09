# FlashUI / HUD static findings — V61.21

The failed V61.19 RTTI global sweep did not mean FlashUI was absent. Static xrefs reveal a direct HUD interface path:

- `WHGame+0x492D938` is loaded as a global interface pointer by HUD/cursor code.
- `WHGame+0x89B6A6` loads that global, then calls its vtable slot `+0x70` with the literal `hud` (RVA `0x3A55B7C`).
- The resulting element pointer is then queried through element vtable slot `+0x238` using `CombatCursorCross` (RVA `0x3C33BE0`).
- `WHGame+0x188ABF3` performs the same global/`hud` lookup and queries `CombatCursorCross`, `CursorCrossPBSlot`, and `CursorCrossOpponentLock`.

This gives a concrete live-interface observer target without guessing the old CryEngine `SSystemGlobalEnvironment::pFlashUI` layout.

V61.21 passively hooks the resolved global object's slot +0x70 and logs only calls whose name is `hud`. It does not call UI APIs itself and does not write or reload UI state.
