# V17 post-fix findings: singleton bug confirmed resolved, new issue found

## Context

Following the `XrManager::Instance()` vs. separately-owned `XrManager`
singleton mismatch fixed in ROADMAP.md MAJOR UPDATE 17, a fresh
`KCD2VR-debug.log` was captured with that fix in place, both safety gates
(`DiagnosticRendering`, `EnableNativeEyeWrite`) still at their defaults
except `EnableNativeEyeWrite=true` for this deliberate test.

## Confirmed: the singleton fix worked

110 total `NATIVE-PAIR-V17 apply=` events, zero crashes, zero errors, zero
hook-layering warnings. Of those, **56 now show genuine live OpenXR data**:

```
xr_ipd=0.068530 left=-0.034265 right=0.034265 source=fresh-xr age_ms=0
view_count=2 flags=0xF locate_result=0 predicted=79487725155725
```

`68.53mm` is a plausible real human interpupillary distance - this is your
actual Quest 3's tracked eye separation, not the `fixed-64mm` hardcoded
fallback every single apply showed before the fix. `locate_result=0`
(`XR_SUCCESS`), `view_count=2`, and a large nonzero `predicted` (a real
`XrTime`) all confirm this is genuine live data.

The remaining 54/110 still show `locate_result=-2`/`source=fixed-64mm`,
roughly a 50/50 split. This is plausibly ordinary cross-thread timing (the
`WaitForFrame` loop and this game-thread hook run on independent paces) and
not necessarily a new bug - the existing tiered fallback logic in
`GetOpenXrStereoInjection` (fresh/cached/relaxed tiers) already anticipates
some staleness. Not flagged as urgent; revisit only if the issue below
turns out to be connected to it.

## New issue: `beforeA`/`beforeB` are zero in ALL 110 applies, including all 56 successful ones

```
camA=0000025E809C0A38 offA=0x8EE8 beforeA=(0.000,0.000,0.000)
camB=0000025E809C1058 offB=0x9508 beforeB=(0.000,0.000,0.000)
```

Even when the OpenXR side is fully working (real IPD, `locate_result=0`),
the GAME-side camera translation being read from the correlated
`CRenderView`'s `+0x8EE8`/`+0x9508` slot is exactly zero, every single time,
across all 110 samples. This is a separate bug from the singleton issue -
fixing the XR data pipeline did not change this at all.

## Two hypotheses, not yet distinguished

1. **Wrong/inactive view targeted.** The correlation logic (`g_native_eye_target`,
   locked via the deferred-snapshot mechanism in `CRENDERVIEW_DEFERRED_STATE_SAFE_V5`)
   may be locking onto a shadow/reflection/portal/auxiliary `CRenderView`
   rather than the actual player-visible main view. This is the exact risk
   `camera_hook.cpp`'s own long-standing TODO calls out (TPVCamera gates
   against this via a `CView` RTTI check we still only partially implement).
   If true, the position would plausibly sit at origin because this
   particular render pass is never actually used to draw the frame you see.

2. **Deferred population, same shape as the V5 bug.** The translation at
   `+0x8EE8`/`+0x9508` might genuinely not be written yet at the exact
   moment `0x866F6C` fires - populated by a later code path in the same
   frame, the same class of timing issue `SAFE-V5` found and fixed for the
   `+0x38` owner-pointer field. If true, a deferred-read (queue this
   `child`+offset pair, re-read on the *next* correlated call, same pattern
   `FlushDeferredCRenderViewSnapshot` already uses) should eventually show a
   real position.

## Recommended next steps, cheapest/most informative first

1. **Cross-reference the same `child` pointer across existing log line
   types.** The `child=0000025E809B7B50` value in `NATIVE-PAIR-V17` lines
   should be directly comparable to the `crv=%p` field already logged by
   `CRV-CHECKOUT-STATE`/`CRV-DEFERRED` (SAFE-V4/V5). If this exact address
   appears there too, check whether ITS logged state/position was ever
   non-zero at any point in the same session - distinguishes hypothesis 2
   (eventually populated) from hypothesis 1 (never populated because it's
   not the real view).
2. **Compare against `camera_hook.cpp`'s own independently-read position**
   (via `kSViewParamsPositionOffset`, already used for tracking-only mode
   and proven reliable over ~20+ hours of testing). If the CView-based
   position is non-zero at roughly the same moment while the CRenderView
   slot is zero, that's strong evidence for hypothesis 1 (different,
   inactive view) rather than hypothesis 2 (same view, just not populated
   yet in this codepath).
3. **If hypothesis 2 looks likely**: apply the same deferred-snapshot
   pattern V5 already established - queue `(child, cam_a, cam_b)` at the
   moment `0x866F6C` fires, re-read on the next correlated call (or on a
   subsequent frame), see if the position has become non-zero by then.
4. **If hypothesis 1 looks likely**: this needs the `CView` RTTI
   verification `camera_hook.cpp` has deferred since the very start of this
   project (checking the embedding `CView`'s vtable/RTTI name, not just
   that it resolves to something inside `WHGame.dll`'s module range) -
   applied to whichever object is being correlated here, to confirm it's
   genuinely the player's main view before trusting anything read from it.

## Safety note, unchanged

None of this changes the risk profile of the existing gates. Both
`DiagnosticRendering` and `EnableNativeEyeWrite` remain real, necessary
safety boundaries regardless of which hypothesis turns out correct - this
investigation is entirely about data correctness (is the camera we're
writing to the RIGHT one, with the RIGHT baseline), not about whether it's
safe to touch it at all.
