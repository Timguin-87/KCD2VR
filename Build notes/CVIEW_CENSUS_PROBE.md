# Exact CView Census + Renderpass Live Probe

Uses RTTI recovered from the user's exact WHGame.dll:
- CView vtable RVA 0x3A7A030
- CView vtable RVA 0x3A7A048

Every CCamera::UpdateFrustumPlanes invocation is observed before modification and grouped by:
- exact engine caller RVA
- CView pointer
- vtable RVA
- exact CView RTTI match
- thread

The build also retains the exact WHGame+0x2525414 render-pass observer.

Key log prefixes:
- CVIEW-CENSUS NEW-EXACT-CVIEW
- CVIEW-CENSUS new-caller
- CVIEW-CENSUS totals
- RENDERPASS-LIVE

No CVar writes. DiagnosticRendering=false.
