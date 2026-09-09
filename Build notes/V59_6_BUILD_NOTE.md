# V59.6 build note

Source-only Windows/MSVC diagnostic build. No Windows binary was produced in this Linux environment.

Changes relative to V59.5 source:
- unique build identity `SQUARE-STEREO-PROBE-20260905-B`;
- zoom range remains 0.80–2.00 in all active UI/apply paths;
- F8 reduced to 4 ms and adaptive smoothing for the tracking comparison;
- remaining F8 phases perform scene render-target archaeology around the already-proven dual `CStandardGraphicsPipeline::Execute` re-entry;
- tiny 10 mm total diagnostic IPD is now permitted for safe separation proof;
- `DeviceHook::ResourceBarrier` logs exact-size normal-pipeline textures that touch RENDER_TARGET, tagged with the thread-local re-entry eye;
- old AFR/PRESENT stereo bridge is explicitly disabled in stereo-probe phases;
- no claim of native geometric stereo until persistent distinct L/R scene captures are proven.
