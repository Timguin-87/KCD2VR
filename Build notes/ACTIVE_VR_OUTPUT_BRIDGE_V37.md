# KCD2VR V37 — Active Game-to-HMD Output Bridge

## Goal
This is the first active output build after the V29–V36 renderer archaeology. The success criterion is intentionally simple: **the completed KCD2 game image appears in the OpenXR headset**. The first bridge is mono-to-both-eyes; true per-eye scene rendering is a later step.

## What changed
- New INI gate: `EnableVrOutputBridge = true`.
- OpenXR swapchains/composition are enabled in normal (non-diagnostic) mode when the bridge is on.
- Hooks `ID3D12Device::CreateCommandList` and `ID3D12GraphicsCommandList::ResourceBarrier`.
- Watches game DIRECT command lists for a transition whose `StateAfter == D3D12_RESOURCE_STATE_PRESENT`; that resource is retained as the latest game presentation texture.
- The mod's own XR command list is explicitly excluded from this capture path (important because D3D12 COMMON and PRESENT are both value 0).
- A tiny fullscreen D3D12 shader samples the captured game texture and renders it into both acquired OpenXR eye images, handling different desktop/HMD resolutions.
- If no valid source is available or the blit fails, the old diagnostic clear is used as a visible fallback rather than submitting an undefined texture.

## Important markers
- `V37-BRIDGE: CreateCommandList hook enabled`
- `V37-BRIDGE: ResourceBarrier PRESENT capture hook enabled`
- `V37-PRESENT-CAPTURE ...` — KCD2 resource transitioned to PRESENT.
- `V37-BRIDGE fullscreen blit pipeline created`
- `V37-BRIDGE-BLIT ...` — game texture was rendered into both XR eye images.
- `V37-BRIDGE no PRESENT-transition source yet; using diagnostic fallback`
- `V37-BRIDGE primary PRESENT-source blit failed; using diagnostic fallback`

## Test configuration
Keep:
```
DiagnosticRendering = false
EnableVrOutputBridge = true
```

If V37 succeeds, expect the ordinary KCD2 image in both eyes. It is intentionally not stereo yet.

## What V37 does NOT claim
- It does not yet render KCD2 twice.
- It does not yet supply asymmetric OpenXR eye projections to the native scene renderer.
- It does not reinterpret the retired mode-2 camera pair as left/right eyes.
- It does not force `CSystem+0x598` or old stereo CVars.

## Next milestone after a successful bridge
Once `V37-BRIDGE-BLIT` is proven and the game is visible in-HMD, the next causality build can split the source into genuinely different left/right scene renders and then inject each eye's OpenXR FOV.
