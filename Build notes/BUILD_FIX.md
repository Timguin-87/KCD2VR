# Parent live probe compile fix

Fixes the MSVC errors from the first parent-live probe:

- `hook::Target` now receives `DetourModKit::Address{target}`, matching the working `device_hook.cpp` usage.
- `__try/__except` moved into `ValidateMultiViewPrologueSeh()`, a tiny SEH helper with no C++ objects requiring unwinding, avoiding C2712.

No target RVAs or intended runtime behavior changed.
