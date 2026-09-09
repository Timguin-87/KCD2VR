# CView Virtual Owner Probe

This build follows the first exact live `CView` recovered from the user's current retail `WHGame.dll`.

## Static anchors

- live `CView` vtable observed at runtime: `WHGame+0x3A7A048`
- vtable slot 2 (`+0x10`) -> `WHGame+0x7F0EC0`
- exact `.pdata` function range: `0x7F0EC0..0x7F1AF9`
- that function accesses the embedded camera at `CView+0xE8` and is responsible for two of the confirmed frustum callsites (`0x7F12E6`, `0x7F1A68`).

## Runtime output

Search `KCD2VR-debug.log` for:

- `CVIEW-VIRTUAL BEGIN`
- `CVIEW-VIRTUAL call=`
- `CVIEW-VIRTUAL caller-summary`
- `CVIEW-CENSUS NEW-EXACT-CVIEW`

The key result is `caller_rva` for `CVIEW-VIRTUAL`. Because `0x7F0EC0` is a vtable function and has no direct `E8` callers, runtime observation is the cleanest way to recover which higher-level ViewSystem/game function invokes it.

The hook calls the original unchanged.

## Camera safety fix included

Tracked head pose is now applied only when `(camera - 0xE8)` has one of the exact RTTI-backed CView vtables (`0x3A7A030` or `0x3A7A048`). The old permissive WHGame-module-range vtable test is removed. `camera_pose_applications` now increments only when a pose was actually applied to an exact CView.

Keep `DiagnosticRendering=false` for this probe.
