# CRenderView downstream correlation probe (SAFE / observer-only)

This build extends the manager-checkout observer without creating or submitting any custom RenderView.

It tags each exact `CRenderView*` returned by the engine-owned manager checkout at `WHGame+0x16F78F4`, then observes:

- renderer world orchestrator `WHGame+0x4E6924` (known 4-argument call site from `0x4E5A57`), and
- the render/pass object query `WHGame+0x4E7E64` (known one-argument integer query).

For each newly observed render/pass object, the probe performs a read-only pointer correlation:

1. scans the render/pass object through `+0x1600` for any tagged CRenderView pointer;
2. scans each tagged CRenderView through its known allocation size `0xA770` for a backlink to that render/pass object;
3. logs selected render/pass state fields (`+0x1AC/+0x1B0/+0x1FD/+0x204/+0x224/+0x1548/+0x154C/+0x1550`).

Important log prefixes:

- `CRV-CORRELATE tag`
- `CRV-CORRELATE new-render-object`
- `CRV-CORRELATE LINK`  <-- decisive direct/backlink relationship
- `CRV-DOWNSTREAM world-orchestrator`
- `CRV-DOWNSTREAM render-object-query`

No CRenderView is manually allocated. No `vr.cfg` flag is forced. No render submission is changed.


## SAFE-V2 crash fix
The `WHGame+0x4E6924` world-orchestrator detour is disabled. The previous four-argument signature was not proven and correlated with MFA `WHGame+0x4E6598` after the first successful return. SAFE-V2 retains manager checkout tagging and the proven one-argument `WHGame+0x4E7E64` render-object query observer.
