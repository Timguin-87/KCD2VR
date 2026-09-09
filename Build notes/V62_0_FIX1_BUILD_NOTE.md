# V62.0 FIX1 build note

Compile-only correction to the original V62.0 source package.

## Fixed

Removed a retired, unreachable FlashUI retry block in `src/mod.cpp` that still referenced the deleted local variables `flashui_installed` and `flashui_retry_tick`. C++ name lookup still occurs inside `if(false)`, so MSVC correctly rejected the original package with C2065.

The old FlashUI observer remains retired in normal play. This fix does **not** reintroduce the generic MasterSwitch or percentage subtitle experiments.

## Behavior

No intended runtime behavior changes from V62.0:
- V61.12 culling correction retained.
- `Fix head sync` remains mode 0 <-> mode 2 (`SYNC_NOW`).
- native semantic UI controls remain in the F7 menu.
- semantic VRIK ownership test remains opt-in.
