# KCD2VR V62.6 test guide

- Main log must contain `KCD2VR BUILD KCD2VR-V62.6-SAFE-GRAPHICS-DIAG-PERSIST`.
- `KCD2VR-diagnostics.log` is created next to the ASI at startup. F9 writes an immediate ON/OFF marker.
- Gamma/Brightness/Contrast are now compositor-side OpenXR adjustments; they never write KCD2 renderer color CVars.
- Tune HUD, open/close Inventory, wait ~1s: a bounded HUD reapply should restore the tuned layout.
- Put `MIS_MENU_BOOK.bmp` next to `KCD2VR.asi`; the supplied manuscript background is used and the left page is kept clear.
- VRIK remains zero-write diagnostic-only. After 10-15 seconds F9 ON, collect both logs.
- V62.5 quick-consumable controls are preserved.
- vcpkg is not required; CMake fetches OpenXR if it is not already installed. DetourModKit remains the one external prebuilt package.
