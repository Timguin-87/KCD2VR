# Multi-hop ViewCamera Consumer Probe

This revision corrects the previous broad `0x426E80` hook. That address is a generic CCamera copy routine and therefore produced huge unrelated traffic.

The probe now hooks the actual CSystem vtable endpoints:

- `WHGame+0x7F0AE0` — vtable slot `+0x430`; unique thunk used by `CViewSystem::Update` to publish the final active camera. It adjusts `this += 0x288` and tail-jumps to the generic camera copy.
- `WHGame+0x5D77F0` — adjacent vtable slot `+0x438`; returns `CSystem + 0x288`, i.e. the persistent ViewCamera address.

The getter hook is especially useful: its caller census tells us which engine subsystems consume the final ViewCamera after CViewSystem publishes it. This can expose renderer / render-view setup without guessing one function at a time.

All hooks are observer-only. Keep `DiagnosticRendering=false`.

Log markers:
- `MULTIHOP PUBLISH-VIEW-CAMERA`
- `MULTIHOP GET-VIEW-CAMERA`
- `MULTIHOP GETVIEW caller-summary`
