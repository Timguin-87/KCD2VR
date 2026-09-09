# Multi-hop CView -> CSystem camera probe

This build intentionally tests several confirmed steps in one launch.

Hooks (all observer-only):
- `WHGame+0x7F0220` CViewSystem update loop.
- `WHGame+0x7F0AEC` active CView linked-entity helper.
- `WHGame+0x7F0B88` CEntity camera/transform path.
- `WHGame+0x7F0DF4` CEntity fallback path.
- `WHGame+0x426E80` CSystem view-camera copy target reached from the CSystem primary vtable slot `+0x430` via thunk `WHGame+0x7F0AE0`.
- `WHGame+0xB4BEFC` CView constructor.

Known exact RTTI/vtables from the uploaded retail DLL:
- CView: `0x3A7A048`
- CViewSystem: `0x405D6C8`
- CEntity: `0x4072C78`
- one primary CSystem vtable: `0x3DC2858`

Log prefix: `MULTIHOP`.

Keep `DiagnosticRendering=false`.
