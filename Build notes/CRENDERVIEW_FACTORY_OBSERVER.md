# CRenderView factory observer — safe rollback

The previous dry-run directly allocated and constructed an orphan `CRenderView`. The game later crashed.

BugSplat reported MFA `WHGame!005df848`. On the exact WHGame.dll this is inside the prologue of a separate engine dispatch routine (`WHGame+0x5DF840`), which is consistent with earlier state corruption / invalid lifecycle rather than a direct fault in the constructor itself.

This build performs NO custom allocation and creates NO second view.

It only observes the game's own paths:
- `WHGame+0x1DED8A0` — generated CRenderView factory closure.
  - closure+0x8 = mode/id
  - closure+0x10 = captured owner pointer
  - allocates 0xA770 and calls constructor
- `WHGame+0xBBCFF0` — real CRenderView constructor.
- exact CRenderView vtable = `WHGame+0x3DF25C0`.

Expected:
- `CRV-FACTORY-OBS enter/leave`
- `CRV-CTOR-OBS enter/leave`

This tells us the real owner/context and caller that must be preserved before attempting a second view through the engine's normal lifecycle.

Keep DiagnosticRendering=false.
