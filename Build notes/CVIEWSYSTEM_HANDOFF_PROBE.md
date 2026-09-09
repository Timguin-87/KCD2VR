# CViewSystem + active-camera handoff probe

Observer-only probe for the exact retail WHGame.dll supplied by the tester.

Hooks:
- `WHGame+0x7F0220..0x7F0A82` — exact `.pdata` owner function containing the confirmed active `CView` virtual update call. Embedded engine strings/RTTI identify this neighborhood as CryAction ViewSystem / CViewSystem.
- `WHGame+0x7F0AEC` — helper called immediately after the active CView update with `CView*` and `CView+0xE8` camera. It forwards the camera to an object stored at `CView+0x418` via virtual slot `+0xF0`.

Prefixes:
- `CVIEWSYS-LIVE`
- `CAMERA-HANDOFF`

Keep `DiagnosticRendering=false`. The probe changes no game state.
