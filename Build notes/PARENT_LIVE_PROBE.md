# Parent + live multiview invocation probe

This build follows the two confirmed direct callers of the surviving KCD2
multiview setup function.

Known call sites for `release_1_5_1308617_856`:

- Caller A: `WHGame + 0x25524BF`
- Caller B: `WHGame + 0x2552FB8`
- Multiview function: `WHGame + 0x254DE70`

The build does two things.

## Static parent windows

It dumps larger code windows around both call sites using the prefix
`PARENT-PROBE`.

## Live classification

It validates the exact known prologue at `0x254DE70`, then installs a lightweight
DetourModKit hook at that function. The detour calls the original function unchanged.

The hook classifies each invocation from `_ReturnAddress()`:

- `0x25524C4` => Caller A
- `0x2552FBD` => Caller B
- anything else => logged as another caller

It logs the first invocation of A/B, Windows thread ID, the incoming `this` pointer,
and periodic counters every 600 total calls.

Important prefixes:

- `PARENT-LIVE hook enabled`
- `PARENT-LIVE first CallerA invocation`
- `PARENT-LIVE first CallerB invocation`
- `PARENT-LIVE counters`
- `PARENT-LIVE other caller`
- `PARENT-LIVE hook disabled`

The original multiview function is still executed on every invocation; this probe
does not enable `r_3MonHack`, alter FOV, or modify render targets.

Use `DiagnosticRendering=false` for this diagnostic run.
