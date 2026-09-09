# V39 quick test

1. Build/install as usual.
2. Keep `VrStereoExperiment = 1`.
3. Start the game and wait until both eye caches report `cacheValid=11`.
4. In the headset, look at a near object against distant scenery and move/rotate your head slowly.
5. Report whether there is real depth, whether the eyes appear swapped, and whether AFR flicker/judder is visible.

Fallbacks:
- Set `VrAfrIpdScale = -1.0` is **not supported**; use `VrStereoExperiment=0` if eye order looks wrong and report it so eye order can be explicitly swapped next build.
- `VrStereoExperiment=0` restores the mono V38/V37 bridge.
