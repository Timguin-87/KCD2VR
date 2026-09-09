# Render-pass live probe

This build uses the uploaded WHGame.dll's `.pdata` exception directory rather
than heuristic function starts.

Exact target:
- WHGame+0x2525414 .. +0x25254FD

Why:
- it is the genuine renderer-side consumer of `e_Recursion`
- the earlier engine-side `e_Recursion` hit was inside CVar registration
- this function initializes/updates a small state object and is called from the
  larger WHGame+0x2526CF4..0x25287E5 renderer function

The hook is observational:
- calls the original unchanged
- logs caller RVA
- logs the four Win64 argument registers for the first 64 calls
- counts calls and callers

Keep DiagnosticRendering=false.

Useful log prefix:
RENDERPASS-LIVE

## Compile fix
The hook target is wrapped in `DetourModKit::Address`, matching the project's existing DeviceHook usage.
