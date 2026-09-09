# V40.1 ResourceBarrier race repair

V40 could install the same ID3D12GraphicsCommandList::ResourceBarrier hook twice when multiple DIRECT command lists were created concurrently. DetourModKit reported a layered-hook collision, then PRESENT capture failed and the XR bridge displayed its purple diagnostic fallback.

V40.1 atomically reserves ResourceBarrier hook ownership with compare_exchange before inline_at()/enable(). The reservation is released only if installation fails.

Validation default is VrStereoExperiment=1 (AFR). Insert still cycles modes; Home still toggles global compositor eye swap. Legacy probes remain disabled.

Expected startup marker:
`V40.1-BRIDGE: ResourceBarrier PRESENT capture hook enabled (single-owner CAS)`

There should be no layered-hook warning and no `ResourceBarrier hook enable failed`.
