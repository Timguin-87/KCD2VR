# V61.4 GOOD MONO + native stereo activation test

- MONO restores the pre-cyclopean per-eye OpenXR pose/FOV submission.
- Failed TP/cull widening is retired even if an old INI still says VrCullFrustumScale=1.25.
- F8 is safe: no hardware breakpoints, no thread suspension.
- F8 sequence: 2s baseline, 4s strength 0.05, 4s strength 0.15, 4s strength 0.15 plus conservative nonzero distances only if the retained values were zero, then automatic restore.
- Watch for any real left/right scene separation, changed parallax, second-eye rendering, or desktop stereo output.
- Send KCD2VR-debug log after one F8 run.

This is source only; no Windows ASI was compiled in the Linux environment.
