# V60.8 Build Note

Build ID: `CAMERA-OFFSET-UI-RESEARCH-20260905-L`

Changes:
- Added local-space camera back/forward slider, -0.30..+0.30 m, positive = backward.
- Added local-space camera vertical slider, -0.30..+0.30 m, positive = up.
- Offsets are independent of tracked HMD translation and FOV; they apply at both legacy frustum injection and the default reversible PublishViewCamera path.
- Persisted as `VrCameraBackOffsetMeters` and `VrCameraHeightOffsetMeters`.
- V60.7 phase-B matched source/submitted FOV mapping is now the regular pseudo-mode baseline; MONO remains on the plain mapping path.
- UI.zip / Engine.pak / WHGame DLL static research documented in `V60_8_UI_ENGINE_FINDINGS.md`.
- No live Scaleform-memory write added yet; next safe UI target is the engine-supported MovieClip PosRotScale path for root HUD movie clip `all`.
