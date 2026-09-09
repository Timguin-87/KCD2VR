# V45 static findings — secondary camera path

Build target: WHGame release around 1.5.6 / current user binary. RVAs are version-specific.

## EViewType mapping
The renderer initializes four CRenderView manager classes with literal names and integer constructor types:

- `Normal View` -> 0
- `Recursive View` -> 1
- `Shadow View` -> 2
- `BillboardGen View` -> 3

This resolves a long-standing ambiguity: old type-2 CRenderView observations were shadow rendering, not a stereo eye.

## GetOrCreateRenderView
`WHGame+0x43EB60` indexes renderer state at `renderer + 0x23D8 + type * 0x118`, then either reuses a view or calls the CRenderView manager checkout. It occupies renderer vslot `+0x1E8` and matches CryEngine's `IRenderer::GetOrCreateRenderView(EViewType)` semantics.

## Recursive pass builder
`WHGame+0x2525414`:
- initializes a rendering-pass object,
- copies/sets the supplied camera,
- invokes the renderer render-view accessor with **type 1**,
- stores the returned render view in the pass around `+0x20`,
- marks the pass recursive,
- applies the `e_RecursionViewDistRatio` setting.

Observed calling convention from the only direct caller:

```cpp
void* __fastcall BuildRecursivePass(
    void* out_pass,
    std::uint32_t thread_or_context_id,
    void* camera,
    std::uint32_t render_flags);
```

V45 only observes this ABI; it does not call it itself.

## Existing render-to-texture owner
The only direct caller of the recursive builder is inside a large renderer function whose embedded debug string is:

`CD3D9Renderer::FX_DrawToRenderTarget`

The block also contains:

`*** End RT for Water reflections ***`

So KCD2 already performs engine-owned secondary camera rendering into a texture through Recursive View. This is the concrete CryEngine analogue to the Cyberpunk VR VRCAM concept.

## Why V45 is observer-only
The existing water-reflection DrawToRenderTarget path carries substantial render-target, clipping, pass, and renderer state. Calling it with guessed arguments would be less safe than confirming its naturally-created type-1 CRenderView and its pipeline/target behavior first.
