# First real second-CRenderView dry run

Static analysis of the user's exact `WHGame.dll` established:

- `CRenderView` RTTI: `.?AVCRenderView@@`
- live vtable RVA: `0x3DF25C0`
- object size allocated by the game's factory lambda: `0xA770`
- allocator: `WHGame+0x4F75C0`
- constructor: `WHGame+0xBBCFF0`
- generated factory lambda `_Do_call`: `WHGame+0x1DED8A0`
- the lambda allocates `0xA770` and calls the constructor.
- constructor arg3 is stored at `CRenderView+0x14`.
- in this exact build constructor arg2 is not consumed by the constructor body.
- render owner: `WHGame+0x4E6924`
- its per-view loop calls `WHGame+0x4E7E64` with the view-sized object in RCX before driving
  `0x4E8524 -> 0x4E8868 -> 0x4E8A18`.

This experiment hooks `0x4E7E64`. When RCX has the exact `CRenderView` vtable, it:
1. records the game's live main `CRenderView*`;
2. allocates `0xA770` with the game's allocator;
3. calls the game's real `CRenderView` constructor;
4. uses the main view's `+0x14` mode/id for the new object;
5. verifies RTTI/vtable and logs `SECOND-CRV DRY-RUN PASS`.

It deliberately DOES NOT insert the second view into the engine's view list and DOES NOT submit it.
That separates “can we create a second genuine engine view?” from the much riskier “can we render it?”

Expected lines:
- `CRV-LIVE ... rtti='.?AVCRenderView@@'`
- `SECOND-CRV allocated=...`
- `SECOND-CRV constructed=... rtti='.?AVCRenderView@@'`
- `SECOND-CRV DRY-RUN PASS ...`

The diagnostic object is intentionally retained for the life of the process in this proof build.
Keep `DiagnosticRendering=false`.
