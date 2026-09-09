# V61.51 VRIK + Head Sync + Live Semantic UI

This branch is V61.50 plus a minimal optional Lua HUD companion.

## Preserved
- V61.12 exact gameplay-CView culling synchronization and immediate restore.
- V60.1/V61.36 dialogue/inventory guard: no automatic `cl_fov` write.
- GOOD MONO/OpenXR presentation baseline.
- VRIK semantic ownership ladder only; no OperatorQueue writes and no old `+0x53D7C4` QuatT assumption.
- High-frequency debug trace remains disabled.

## Head sync
F7 `Fix head sync` is the requested binary control:
- OFF -> pose source 0 (`CACHED_XR`)
- ON -> pose source 2 (`SYNC_NOW`)

FPS limiting remains independent. The reported smooth headset combination was game cap 84-85 plus `Fix head sync` ON.

## UI
F7 also contains `VR UI layout`, a master command for the optional `KCD2VR_UI_LIVE` Lua companion.

The companion uses vanilla/current KCD2 HUD.xml MovieClips via `UIAction.GetPos/GetScale/SetPos/SetScale`:
- `Subtitles` = `bc.subtitles`
- `DialogLeft` = `cl`
- `DialogRight` = `cr`
- `Hints` = `br.actionHints`

It does not replace `hud.gfx` or HUD.xml and does not require Dynamic HUD/Dynamic Detector.
Default test layout:
- subtitle scale 50%
- dialogue scale 67%, 12% inward
- hints scale 100%, 30% inward

Letterbox removal is explicitly NOT claimed in V61.51.
