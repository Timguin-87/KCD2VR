# V60.8 UI / Engine static findings

Source material inspected: user-packaged UI.zip (from Libs/UI / IPL_GameData.pak), Engine(3).pak, WHGame(3).dll and WHGameArm(3).dll.

## Strong UI clues
- `UIElements/HUD.xml` declares `hud.gfx` as a fullscreen GFx layer and exposes `SetRatioPreset -> fc_setRatioPreset`.
- HUD.xml exposes named MovieClips including `MasterSwitch=all`, `Subtitles=bc.subtitles`, `Compass=tc.compass`, `Stats`, action hints, dialogue clips, cursor, ratio strips, etc.
- CryEngine binary strings expose generic UI flowgraph actions `UI:MovieClip:PosRotScale` and `UI:Template:MovieClip:PosRotScale`, plus the description `Pos/Rot/Scale access to MovieClips`. This is the most promising engine-supported route for runtime HUD scaling/translation without rescaling the 3D image.
- Binary strings also describe pseudo-3D MovieClip depth behavior and require a MovieClip to sit in a parent manually centered on screen.
- `screen_resize`, `fullscreen`, `fixdyntexsize` and enum `Fixed=0,Fullscreen=1,Dynamic=2` are present in the retail binaries, matching the XML constraint system.
- `wh_ui_ApseCameraPosOffset` exists, but its help string says it moves the APSE camera anchor and is therefore menu/inventory specific, not a general HUD scale control.
- `wh_cs_camera_offset` is explicitly a combat-camera Y-axis offset, so it is not used for the new general VR camera calibration.
- `cl_tpvDist` / `cl_tpvYaw` are third-person controls and are likewise not appropriate for the first-person VR camera.

## Next UI implementation target
Prefer engine-supported MovieClip/root transform over editing the captured framebuffer. First candidate is root HUD MovieClip `all` via the engine's PosRotScale UI action/interface. Read-only discovery / call-path tracing should precede any direct pointer write. If a safe callable path is found, expose HUD scale and X/Y position sliders independently of world projection.
