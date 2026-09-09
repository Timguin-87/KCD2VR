# V40.2 Config Repair

V40.1 proved the ASI-side INI was found but DetourModKit reported `Loaded 0 items`, leaving newer VR settings at defaults.

V40.2 explicitly reads every active `[Settings]` VR key through Win32 `GetPrivateProfileStringA` from the INI next to the loaded ASI.

Validation defaults in the packaged INI:
- `VrStereoExperiment = 1`
- `VrPresentationMode = 1`
- `VrPresentationZoom = 0.5`
- `VrAfrIpdScale = 0.2`
- `VrSwapEyes = false`
- `EnableLegacyStereoProbes = false`

The debug log must show explicit INI parse lines and then matching effective config values.
