# KCD2VR V20 — persistent XR good-cache + dual generated-state trace

V20 is based on the uploaded fixed-V17 scaffold.

## Confirmed ownership fix
`Mod` uses `XrManager::Instance()`; native stereo injection reads the same singleton.

## XR cache correction
V17 wrote every `xrLocateViews` result into one snapshot. A transient failed locate therefore destroyed the most recent good stereo pair and made the injection fall back to fixed 64 mm until another successful locate arrived.

V20 keeps two snapshots under the same mutex:
- `latest_stereo_views_`: newest raw locate result, including failures, for diagnostics.
- `last_good_stereo_views_`: newest plausible two-eye pair only; failures never overwrite it.

Injection selection:
1. fresh latest valid pair (<=80 ms)
2. cached last-good pair (<=500 ms)
3. relaxed latest successful two-view pair (<=750 ms)
4. fixed 64-mm diagnostic fallback

## KCD2 generated-state trace
The pair hook now dumps both generated substates:
- first state at `out + 0x000`: `q0..q11`
- second state at `out + 0x290`: `s2q0..s2q5`

This is intended to distinguish two hypotheses in one run:
- If q2 contains the left displacement and the second substate contains the right displacement, the zero input translations are likely relative cameras and the next target is the downstream consumer/render routing.
- If only the first substate changes or the second does not reflect camB, revisit the builder/target correlation.

No 0x4E6924 or 0x4E7E64 hooks are enabled.
