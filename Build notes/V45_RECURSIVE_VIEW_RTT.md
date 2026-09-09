# V45 — CryEngine Recursive View / Render-to-Texture Proof

## Why this build exists
The previous stereo experiments repeatedly treated CRenderView "mode 2" as a possible stereo child. Static analysis now resolves the constructor EViewType labels in this KCD2 build:

- 0 = Normal View
- 1 = Recursive View
- 2 = Shadow View
- 3 = BillboardGen View

Therefore the old mode-2 route was a SHADOW view, not an eye.

## Strong static mapping
- `WHGame+0x43EB60` behaves as `IRenderer::GetOrCreateRenderView(EViewType)` and selects one of four per-type managers.
- `WHGame+0x2525414` constructs a recursive rendering pass and explicitly asks `GetOrCreateRenderView` for type **1**.
- Its only direct caller is inside `CD3D9Renderer::FX_DrawToRenderTarget`.
- That block contains the debug marker `*** End RT for Water reflections ***`.

This proves KCD2 already has an engine-owned secondary camera render-to-texture route: the same class of mechanism used by Cyberpunk VR's VRCAM, although KCD2 uses CryEngine Recursive View rather than a REDengine RenderToTextureCameraComponent.

## Runtime markers
V45 is observer-only for the recursive path. It does NOT manually construct a CRenderView and does NOT invoke the recursive renderer by itself yet.

Look for:

```
V45-RECURSIVE renderer-global=...
V45-GET-RENDERVIEW ... requested=1(Recursive) ... actualType=1
V45-RECURSIVE-PASS ... renderView=... viewType=1 ...
V45-PIPELINE ... viewType=1(Recursive) ...
```

The decisive result is the same type-1 CRenderView being built by the recursive-pass helper and later appearing in `CStandardGraphicsPipeline::Execute`.

## Important correction
`CRenderView+0x14` is the constructor EViewType.
`CRenderView+0x8EE0` is the selected camera-bank index/selector and is NOT the view type.

## Next active step if V45 confirms the path
Repurpose the engine-owned Recursive View pass for the right eye:
1. Build a recursive pass from a copied gameplay camera.
2. Give it the right-eye pose and a symmetric frustum large enough for the OpenXR eye.
3. Bind a dedicated VR render target through the same DrawToRenderTarget route.
4. Let KCD2 create/configure/render the type-1 CRenderView itself.
5. Capture MAIN as left eye and Recursive View target as right eye.
6. Crop to each runtime XrFovf bounds at submission, following the Crysis VR strategy.

No orphan CRenderView construction and no double simulation tick.

## GPU target correlation
While `CStandardGraphicsPipeline::Execute` is running a type-1 Recursive View, V45 marks the current thread as recursive. The D3D12 ResourceBarrier hook then logs transitions recorded on that same thread:

```
V45-RECURSIVE-RESOURCE ... resource=... size=... fmt=... before=... after=...
```

This should expose the recursive color/depth targets without assuming one render thread. Repeated transitions involving `RENDER_TARGET` state (`0x4`) are especially interesting for the secondary eye color source.
