# Renderer/View Nexus Probe

Read-only follow-up to the theory sweep.

Targets:
- `r_ShowRenderTarget` runtime help-path xref at WHGame+0x25477C6
- `e_Recursion` refs at WHGame+0x2525473 and +0x15C40DB
- `e_SkipInFrustumOptimizationWH` refs
- 3Mon function WHGame+0x254DE70
- both known 3Mon parent ranges

The probe guesses enclosing function boundaries, enumerates direct callees/callers,
and intersects those callgraphs with the two 3Mon parents.

Watch for:
- VIEW-NEXUS anchor
- VIEW-NEXUS caller
- VIEW-NEXUS INTERSECTION
- VIEW-NEXUS shared-parent-target

No CVar writes. No stereo enabling. Keep DiagnosticRendering=false.
