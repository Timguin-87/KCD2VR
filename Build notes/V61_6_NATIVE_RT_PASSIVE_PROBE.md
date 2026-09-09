# V61.6 passive native-stereo render-target probe

This keeps the V61.5 GOOD MONO + pseudo-stereo test path unchanged and adds one new read-only native-stereo archaeology layer.

## New probe

The game D3D12 device is observed at two concrete API points:

- `ID3D12Device::CreateRenderTargetView` — records CPU RTV descriptor handle -> actual `ID3D12Resource*`.
- `ID3D12GraphicsCommandList::OMSetRenderTargets` — resolves bound RTV handles back to real resources and logs full-resolution Normal-view targets.

It performs no CVar writes, no FlowGraph calls, no hardware breakpoints, no thread suspension, and no renderer state changes.

Expected startup markers:

```
V61.6-NATIVE-RT CreateRenderTargetView observer armed
V61.6-NATIVE-RT OMSetRenderTargets observer armed
```

Interesting runtime marker:

```
V61.6-NATIVE-RT-BIND ... presentSerial=... slot=... resource=... size=... fmt=...
```

What matters is whether two stable full-resolution resources are bound in a same-frame / same-present interval in a pattern compatible with left/right eye targets. Ordinary swapchain/backbuffer alternation across different PRESENT serials is not evidence of stereo.

## User test

No extra native-stereo button is required. Just play normally for 30-60 seconds while trying the pseudo modes with F8 if desired. Send the debug log afterward.
