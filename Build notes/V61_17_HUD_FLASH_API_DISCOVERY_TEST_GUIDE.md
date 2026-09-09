# KCD2VR V61.17 — HUD Flash API discovery

Source-only diagnostic branch. No native HUD transform writes are made yet.

## Why this build
The current retail `WHGame.dll` still contains CryEngine FlashUI, including `CFlashUI`, `CFlashUIElement`, `CFlashUIMCPosRotScaleNode`, `CFlashUIVariableNode`, `SetVariable`, and `setVariableDouble`. Current `HUD.xml` exposes `MasterSwitch` -> `all`, plus the action/combat cursor movie clips.

CryEngine's documented C++ route is `gEnv->pFlashUI->GetUIElement("HUD")->GetMovieClip("MasterSwitch")`, then modify the MovieClip display transform. V61.17 performs only the missing pointer discovery step.

## Test
1. Start game and load into normal gameplay.
2. Leave it running 10–20 seconds.
3. Open/close the VR menu once if desired.
4. Send `KCD2VR-debug*.log`.

## Log target
Look for:

```
V61.17 BUILD ID HUD-FLASH-API-DISCOVERY
V61.17-HUD-FLASH-DISCOVERY candidate global_rva=... rtti='.?AVCFlashUI@@'
```

The one-time global RTTI sweep is extended from `WHGame+0x492D900` to `+0x492D980`. It is read-only and SEH-protected through the existing safe-read/RTTI helpers.

## Next build once the pointer is confirmed
Expose real live controls in the VR menu:
- HUD Scale
- HUD X
- HUD Y
- Cursor X
- Cursor Y
- Reset original

Target the XML logical clips `MasterSwitch` (`all`) and `Cursor` (`cccursor.actioncursor`) separately. This avoids the failed HUD.xml constraint/cropping approach and does not alter game FOV/projection.
