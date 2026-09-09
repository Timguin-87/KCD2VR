# KCD2VR V61.14 — UEVR-style native stereo static RE

Target: retail `WHGame(4).dll`, image base `0x180000000`.

This pass is intentionally **static only**. It adds no runtime hook and performs no stereo state writes.

## Why this pass

Praydog/UEVR gets true stereo by taking control at Unreal's eye/view interface, before the final image exists. The equivalent KCD2 question is therefore not "where are StereoL/StereoR?" but:

> Where does CryEngine decide whether stereo is active, which eye/pass is being rendered, and which view/projection belongs to that eye?

## Stock CryEngine reference

Public CryEngine renderer interfaces historically expose both `IRenderer::IsStereoEnabled()` and `IRenderer::GetIStereoRenderer()`. CryEngine documentation also defines `r_StereoMode=1` as **Dual rendering** and retains per-eye separation/convergence/HUD parameters.

That is the architectural analogue to UEVR's use of Unreal's `IStereoRendering` path.

## KCD2 result 1 — the parameter half of native stereo is definitely still present

KCD2 still contains and registers this renderer-side cluster around RVA `0x1017840`:

- `r_stereoScaleCoefficient`
- `r_StereoStrength`
- `r_StereoEyeDist`
- `r_StereoScreenDist`
- `r_StereoNearGeoScale`
- `r_StereoHudScreenDist`
- `r_StereoGammaAdjustment`
- `r_StereoEnableMgpu`
- `r_StereoFlipEyes`

The retained help text for `r_StereoEnableMgpu` explicitly says "dual rendering".

Exact useful addresses already recovered:

- renderer CVar registration block: RVA `0x1017840` onward
- `r_StereoStrength` name RVA `0x3DE8A18`
- `r_StereoEyeDist` name RVA `0x3DE8A70`
- `r_StereoScreenDist` name RVA `0x3DE8AC8`
- `r_StereoNearGeoScale` name RVA `0x3DE9030`
- `r_StereoHudScreenDist` name RVA `0x3DE90D8`
- `r_StereoGammaAdjustment` name RVA `0x3DE9128`
- `r_StereoEnableMgpu` name RVA `0x3DE8D18`
- `r_StereoFlipEyes` name RVA `0x3DE8DB0`

KCD2 also retains the game-side `g_stereoFrameworkEnable` CVar and the FlowGraph nodes `Stereo:StereoParameters` / `Stereo:ReadStereoParameters`.

## KCD2 result 2 — the old public activation CVars appear to have been removed

A full ASCII + UTF-16 string scan of the supplied current `WHGame.dll` finds **no**:

- `r_StereoMode`
- `r_StereoDevice`
- `r_StereoOutput`
- `IStereoRenderer`
- `CStereoRenderer`
- `StereoEye` / `LeftEye` / `RightEye`

This is an important difference from stock CryEngine documentation, where `r_StereoMode=1` is the normal dual-render activation switch.

So KCD2 has retained the **stereo parameter/control layer** while apparently stripping or hiding the old public **activation/output layer**.

That explains why forcing the surviving parameter CVars never created a second scene render.

## KCD2 result 3 — renderer CVar backing storages are registration-only

Static RIP-relative xref scanning of the renderer backing variables found only the registration block references:

- strength storage `WHGame+0x50DBDE8`
- eye-distance storage `WHGame+0x50DBDCC`
- screen-distance storage `WHGame+0x50DBDC8`
- near-geometry storage `WHGame+0x50DBDC4`
- HUD-distance storage `WHGame+0x50DBDC0`
- gamma storage `WHGame+0x50DBDD4`
- MGPU storage `WHGame+0x5571030`
- flip-eyes storage `WHGame+0x5571034`

There are no additional direct code xrefs to those float storages in `.text`.

Therefore a surviving renderer stereo object, if present, is not simply reading those globals by fixed address. Likely alternatives are cached `ICVar*` objects, renderer object fields, or a stripped/internal stereo state structure.

## KCD2 result 4 — all direct name xrefs are understood

`r_StereoEyeDist`, `r_StereoScreenDist`, and `r_StereoHudScreenDist` direct name references resolve to:

1. renderer CVar registration;
2. `CFlowNode_StereoParameters` constructor and `ProcessEvent`;
3. `CFlowNode_ReadStereoParameters::ProcessEvent`.

`r_StereoFlipEyes` direct name references likewise resolve to registration / FlowGraph behavior.

No additional direct-name consumer was found that looks like a scene renderer or eye-pass selector.

This strongly narrows the remaining target: the true stereo path, if retained, is behind an object/vtable/state interface rather than CVar-name lookup.

## KCD2 result 5 — renderer vtable gives us a concrete interface anchor

The current renderer implementation vtable is already known at:

- renderer vtable RVA `0x3DFC008`

The known current `GetOrCreateRenderView` implementation:

- RVA `0x43EB60`

is exactly **vtable slot 61** in this current renderer object.

That is useful because stock CryEngine's public `IRenderer` interface includes, in the same architectural family:

- `GetOrCreateRenderView(...)`
- `IsStereoEnabled()`
- `GetIStereoRenderer()`
- `IsStereoModeChangePending()`

The next static pass can therefore work from a real current vtable anchor instead of guessed signatures.

## UEVR comparison / what changed in our search strategy

UEVR's useful lesson is not to search for a final left/right texture first. It hijacks the engine's eye/view generation contract.

For KCD2 the desired chain is now:

```text
renderer object
  -> stereo-enabled / stereo-renderer interface
     -> eye/pass state
        -> per-eye camera + projection
           -> normal CRenderView scene render
```

rather than:

```text
final backbuffer
  -> look for a second texture
```

## Current verdict

### Proven retained

- native stereo parameter CVars;
- game stereo framework CVar;
- stereo FlowGraph read/write nodes;
- current `CRenderView` renderer machinery;
- historic CryEngine interface architecture that normally exposes a stereo renderer;
- shader-level stereo remnants.

### Not yet proven retained

- a live current KCD2 `IStereoRenderer` object;
- a current eye index / left-right pass selector;
- dual scene invocation on one simulation tick;
- current StereoL/StereoR scene render targets.

### Strong negative evidence

The stock public activation strings (`r_StereoMode`, `r_StereoDevice`, `r_StereoOutput`) are absent from the current KCD2 binary. This means another CVar ladder is not the right next experiment.

## Next exact static targets

1. **Map the current `IRenderer` vtable around slot 61** using stock CryEngine interface ordering/signatures, then identify candidate implementations for:
   - `IsStereoEnabled()`
   - `GetIStereoRenderer()`
   - `IsStereoModeChangePending()`

2. For any candidate pointer-returning stereo getter, trace the returned object's vtable statically and look for methods that:
   - accept/return an eye enum or small integer;
   - modify a `CCamera` / `CRenderView` projection;
   - set viewport/render target state twice;
   - branch on a two-state eye/pass field.

3. Trace callers of `GetOrCreateRenderView` slot 61 and the already-known graphics-pipeline entry around RVA `0x57278C` looking specifically for **two same-frame Normal-view invocations** or an eye/pass field.

4. If no stereo object survives, pivot to the UEVR fallback architecture: **same-tick synchronized sequential rendering**. That means left render + right render without advancing game simulation, not AFR.

## Runtime rule

Do not add another native-stereo runtime write/probe until one of the above static chains identifies a concrete current eye/pass interface. No hardware watchpoints.
