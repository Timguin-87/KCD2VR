# V61.2 DEPTH-PROBE

Changes from V61.1 prep:

- Added a dedicated read-only `DepthProbe` at:
  - `WHGame+0x7BB6B0` CSceneGBufferStage::Execute
  - `WHGame+0x7BC700` six-argument GBuffer MRT setup
- Probe only inspects the Normal CRenderView (`viewType == 0`).
- Logs the WHGame depth wrapper and scans its first 0x100 bytes for D3D12 COM resource candidates.
- Candidate resources are logged with wrapper offset, dimension, width/height, array size, mip count, DXGI format, sample count and resource flags.
- No resource is retained, transitioned, copied or modified.
- TP/KCD2Tools persistent FOV/cull experiment is disabled by default for this depth test by restoring `VrCullFrustumScale` default to `1.0`.

Expected marker:

`V61.2-DEPTH-PROBE armed: Normal GBuffer depth wrapper -> D3D12 resource scan; read-only`

Then `V61.2-DEPTH ... resource=...` or `resource=unresolved` lines.
