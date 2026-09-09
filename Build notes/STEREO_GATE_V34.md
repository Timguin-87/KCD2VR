# V34 — Stereo flow-node boolean gate probe

Read-only probe guided by a comparison with CryEngine's `IRenderer` stereo interface.

## Static KCD2 finding

The surviving KCD2 stereo flow-node code around `WHGame+0x368D458` performs:

1. load object pointer from `WHGame+0x492D8C0`;
2. load its vtable;
3. call virtual slot `+0xF8` with no explicit arguments;
4. immediately `test al, al` and skip the stereo operation when false.

That is a strong structural candidate for an `IsStereoEnabled()`-style predicate. V34 resolves the object and slot at runtime and hooks the resolved function only to log the original boolean result and caller. It never changes the result.

## Markers

- `STEREO-GATE-V34 static`
- `STEREO-GATE-V34 call=`
- `STEREO-GATE-V34 summary`
- `PIPELINE-V34`

If the gate returns false during normal gameplay/rendering, the next step is to identify what fields/object state feed it. If it returns true, this candidate is not the master mono/stereo gate and we continue through the stereo interface object.
