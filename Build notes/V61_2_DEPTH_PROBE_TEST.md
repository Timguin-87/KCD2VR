# V61.2 DEPTH PROBE TEST

Purpose: identify the actual D3D12 resource behind the Normal CSceneGBufferStage `depth` wrapper before implementing depth-based pseudo stereo.

## Safety / isolation
- Probe is read-only.
- Only the Normal CRenderView (`viewType == 0`) is logged.
- The TP/KCD2Tools persistent FOV/cull experiment is NOT enabled by default; `VrCullFrustumScale` default is restored to 1.0 for this test.
- No depth resource is retained or modified.

## Test
1. Start the game and load a normal gameplay save.
2. Stand still for 10-20 seconds, then look around indoors/outdoors for ~30 seconds.
3. No special button is required.
4. Exit and send `KCD2VR-debug*.log`.

## Expected log
At startup:
`V61.2-DEPTH-PROBE armed: Normal GBuffer depth wrapper -> D3D12 resource scan; read-only`

Then either a resolved candidate such as:
`V61.2-DEPTH ... wrapper=... resource=... wrapperOff=... dim=2 size=1920x1800 ... format=... flags=...`

or:
`V61.2-DEPTH ... resource=unresolved ...`

A stable texture-sized candidate recurring at the Normal GBuffer stage is the signal needed for the next grayscale DEPTH VIEW build.
