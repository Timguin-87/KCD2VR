# D85R5R3 — Fresh Pose Uncapped and verified Native launcher save

3 November 2026

## What this revision delivers

This revision adds the requested **Fresh Pose Uncapped** option to the VR menu
and strengthens the launcher so an explicit **Native (no upscaling)** save must
write and verify `r_SuperResolution_Mode = 0` before it can report success.

The source marker is
`KCD2VR-D85R5R3-FRESH-POSE-UNCAPPED-20261103`. The work starts from the completed
R2 corrective source and retains its repairs. The package offers both the
cumulative original-D85R5 overlay and an exact R2-to-R3 overlay. It contains no
new DLL, ASI, generated companion PAK or full diagnostic recordings.

The new mode is an explicit choice, not a default migration. The existing
Fresh Pose option remains available, and its missing-key default remains 2.
The current starter INI is byte-identical to R2, including calibration, hand
settings and the independently saved FPS-limiter preference. The patch does
not infer the user's earlier runtime settings from an exit-saved INI.

## Where to select it

Use either existing menu route:

- **Home > VRIK EXPERIMENTS > Fresh Pose Uncapped**.
- **Home > EXPERIMENTS / LIVE SETTINGS > VRIK+ > Motion timing**.

These pages retain their existing developer-mode visibility. The classic page
has four separate buttons with distinct hitboxes; the live-settings row cycles
through the same four choices. Selection applies live and uses the existing
settings-save path. Returning to **FRESH POSE timing** restores the prior paced
route without requiring a restart.

| Visible choice | Saved `VrMotionTrialD67` | Meaning |
| --- | ---: | --- |
| BASELINE motion | 0 | Existing baseline. Other explicitly selected legacy gates retain their prior behavior. |
| FRESH POSE timing (F7 rung 2) | 2 | Existing fresh-publication pacing; still the default. |
| Fresh Pose Uncapped | 4 | Bypass native-frame XR-token waits and attempt an extra coherent pose refresh before the native update. |
| ELBOW reach preference (F7 rung 3) | 3 | Existing elbow comparison and timing behavior. |

F7 recording rungs remain 0 through 3. Persistent mode 4 is independent of the
finite recording rung: starting F7 rung 2 does not secretly restore the paced
wait. F7 can still observe or compare the elbow behavior under the existing
rules. Switching between persistent 2 and 4 does not change the ordinary arm
policy or reset its continuity. An unrelated invalid INI value is not clamped
upward into the new mode; only an explicit 4 selects it.

## What the recordings actually establish about 90 FPS

There is a concrete wait in the current Fresh Pose implementation. Its
`WaitForNextRenderFrameToken(token, 20, true, false)` call consumes an available
XR publication or waits for the next one, with a 20 ms timeout. The forced
argument allows this path even when the ordinary XR gate setting is zero.
Turning off the manual `sys_MaxFPS` limiter therefore does not remove this
separate synchronization.

The supplied main `KCD2VR.log` contains the following runtime observations.
Line numbers refer to the original recording, not an exit profile:

| Original line | Recorded time | Native frame | Returned XR token | CPU token wait |
| ---: | --- | ---: | ---: | ---: |
| 1346 | 00:03:45.892 | 19546 | 21747 | 6.3317 ms |
| 2346 | 00:03:46.392 | 19591 | 21792 | 6.5220 ms |
| 3285 | 00:03:46.892 | 19636 | 21837 | 5.8640 ms |

Each half-second interval advances both counters by 45. Nearby F2 windows at
original lines 38073, 41231 and 44555 report game FPS, XR FPS and fresh frames
all at 90, with zero old-pair reuse in those windows. Across 646 distinct
sampled timing receipts, the median token wait is 5.9665 ms, the nearest-rank
95th percentile is 7.4352 ms and the maximum is 10.4777 ms. Of those samples,
526 exceed 1 ms. The package includes the compact extraction and original line
references in `evidence/root/TIMING_LOG_RECEIPTS.json`.

This is evidence of cadence coupling, not proof of a literal hardcoded 90 FPS
limit. Available tokens can return immediately, and the recording also contains
windows above 90. These are sampled CPU waits; they must not be added to GPU
scope measurements or treated as a uniform cost on every frame.

The earlier performance regression is a different matter. The user explicitly
reported that H50 and D52 were the same scenes and that the drop from C71
performance began before Fresh Pose. That history is retained. The pacing
source was unchanged between the tested D85R5 source and the R2 corrective
source. R3 removes this identified wait only when the new choice is selected;
it does not claim that doing so restores the earlier 110 FPS result.

## Why head smoothness did not automatically fix the hands

The current source already samples controller poses alongside the head in its
synchronous camera path. The missing distinction is where that newer sample
is consumed. A camera update can use a newer head pose while an arm skinning
job or held-object transform has already been produced from an earlier family.
Publishing another tracking sample alone does not regenerate those finished
outputs.

The OpenXR runtime also owns its frame timing and may apply head-related
reprojection to a completed image. That helps explain why a stable head view
can coexist with visibly uneven moving objects. It is not evidence that the
same treatment was applied to Henry's skinned hands or weapon geometry.

R3 therefore adds acquisition at the existing pre-update boundary, where the
ordinary native update, physics and later VRIK work can still consume it. The
successful later head refresh is retained. The camera is not forced to use the
earlier acquisition merely to make the CPU timestamps equal.

The final native palette/resource/held-object draw-to-eye join remains outside
this change. Existing diagnostics observe CPU publication and some palette
boundaries; they do not prove the final Henry mesh draw consumed a particular
sample. This revision is not a new late GPU skinning upload or a full controller
late-latching implementation.

Primary references used for the timing distinction:

- Khronos, `xrWaitFrame`: runtime frame pacing and predicted display time —
  https://registry.khronos.org/OpenXR/specs/1.1/man/html/xrWaitFrame.html
- Khronos, `xrLocateSpace`: locating a space for an explicit target time, with
  validity and tracking flags —
  https://registry.khronos.org/OpenXR/specs/1.1/man/html/xrLocateSpace.html
- Meta, Asynchronous Timewarp on Oculus Rift: head correction and the separate
  visibility of animation/object judder —
  https://developers.meta.com/vr/blog/asynchronous-timewarp-on-oculus-rift/
- Meta, UE4 Late Latching: a reference for the separate controller/object
  transform and GPU-consumption problem, not a drop-in KCD2 switch —
  https://developers.meta.com/horizon/blog/ue4-late-latching-for-oculus-quest-design-benefits-and-how-to-get-started/

## What the new path does, in order

At the existing outer native-frame entry, before the ordinary update, mode 4
claims that native frame and attempts one pose refresh. Reentry, the extra-eye
pass, the later camera invocation and a failed first acquisition do not create
a second early attempt in the same frame.

The refresh uses the existing camera policy:

| Existing camera source | Mode-4 early refresh |
| --- | --- |
| 0: cached XR worker | Retain the existing cached head/family; do not attach independently advanced hands to the cached head. |
| 1: predicted | Use the current runtime-supplied predicted target without advancing it; refuse stale or unknown provenance. |
| 2: synchronous NOW | Convert current QPC time to OpenXR time and locate the head and both grips at that common base target. |
| 3: offset | Preserve the existing offset-camera semantics and disabled coherent-family selection. |

No camera-source setting is changed. The ordinary NOW source receives the new
early acquisition. With cached/offset sources, the wait is still bypassed, but
the additional synchronous family acquisition is deliberately not substituted
for their different camera policy. Their explicit diagnostic reasons make that
distinction visible.

The manager pose lock is try-only in mode 4, including the existing synchronous
camera call. The controller pair uses the existing try-only action-space lock.
A busy lock returns to the existing completed snapshot; it does not wait until
the next XR frame. The OpenXR lookup itself still has runtime cost, which this
patch does not pretend to eliminate.

The head and grips share a base target. If the user already selected optional
D69 extra hand lead, the same existing helper, velocities, policy identity and
per-hand target metadata are preserved. R3 does not enable extra lead, increase
it, replace it with another prediction algorithm or silently turn it off.

Acquisition performs pose-only reads. It does not call `xrSyncActions` again,
produce another button edge, run gameplay twice, or issue duplicate haptics.
The original action/permission packet keeps its original age. A newer pose
cannot revive an expired permission or turn a reacquired hand into a fresh
strike.

If the grip pair cannot be sampled, the extra early attempt publishes neither
a new complete family nor a new head half. If sampling runs but a hand is
invalid or untracked, the ordinary per-hand held/grace rules apply. Last-good
tracking ages are not renewed by failure. A receipt says whether the family
was actually published and includes its validity/tracking masks; successful
publication does not mean every hand was physically tracked.

Review also closed source-transition and stale-target edges. The tracking
epoch is captured before source validation and rechecked after acquisition.
Changing camera source or disabling coherent tracking during acquisition
cannot commit the old family under the new source. For predicted mode, both
the supplier timestamp and target age are checked. A resumed XR worker's
separate atomics cannot make an obsolete target look newly supplied.

After the attempt, both early and late pacing entry points return without the
old token wait. The separate legacy camera gate also excludes mode 4. Camera
copy/restore and expected-Present ownership remain in place, as do XR frame
submission, eye order, projection, IPD and the retained temporal-upscaler work.
No new renderer mode or AFR switch is introduced.

## Native launcher selection: exact repair and evidence limit

The required user flow is **Start KCD2VR.cmd > Native (no upscaling) > Save &
start**. The expected saved assignment is:

```ini
r_SuperResolution_Mode = 0
```

The supplied source already mapped Native to integer 0. Independent inspection
and a source-connected fixture confirmed that 0 survives the combo item data,
the disabled Native quality control, the Save & start handler, configuration
generation and the arguments passed to the pending-profile save. There was no
demonstrated zero-as-false mapping bug in those paths.

There was a weaker success condition: the old writer checked the stream before
closing it and accepted successful replacement without reading the destination
back. R3 strengthens this into a verified save:

1. Write and flush the staging file, close it, and check the close result.
2. Read the staged file and verify every explicitly generated graphics value.
3. Independently verify the selected upscaler itself, including Native 0. This
   catches a future generator error that omits the mode from its output list.
4. Preserve the existing original-file backup and replace `user.cfg`.
5. Read the installed file and verify those values again before returning
   success to the setup window.

Any failure follows the existing error branch before pending settings are
saved or launch is permitted. The message now identifies write/verification
failure. The existing updater removes duplicate active assignments and appends
the explicit choice. Comments, unrelated config and limiter values remain.
Selecting Native need not delete old DLSS/FSR quality values: mode 0 is what
disables the vendor backend.

Keep is still -1. **Keep current supported setting** leaves the mode assignment
alone, and **Keep settings & start** retains its existing discard-edits
semantics. No assumption is made that the user chose either of those controls
in the failed run. The uploaded `user(2).cfg` contains mode 0, but that snapshot
does not establish what the running renderer used at another time.

The failed run's exact cause remains unestablished. These checks establish the
file saved by the launcher; they cannot establish which installation Steam
starts or prevent an unrelated later writer from changing that file. This
revision adds no repeated runtime CVar enforcement that would fight later
graphics-menu choices. The `.cmd` itself invokes the setup export from the
installed mod binary, so the source repair must be integrated into that binary;
editing the two-line `.cmd` is not the implementation of its dropdown.

## Diagnostics and performance discipline

The new mode operates with diagnostic recording off. Its additional diagnostic
duration clocks, formatting and log writes are skipped when F2 and F7 are off.
Clocks needed to locate poses and preserve actual sample age remain gameplay
data and continue to run.

F2 uses the existing bounded ownership queue and identifies
`UNCAPPED_POSE_LOCATE_NO_TOKEN_WAIT`. F7 emits a rate-limited
`VRIK_F7 FRESH_POSE_UNCAPPED` receipt with the frame, source, reason,
publication result, locate duration, sequence, original timestamps, requested
and completed targets, and validity/tracking masks. `waitNs=0` means no token
wait; `meshSubmission=UNOBSERVED` keeps the later draw limitation explicit.

The existing synchronous-camera failure message is also F2/F7-gated while
mode 4 is selected, so ordinary try-lock contention does not become an idle
debug-log producer. All added receipts use the existing central log and finite
capture lifetimes. The launcher uses its existing error dialog, not a new log.

## Source changes and verification

| Source area | Change |
| --- | --- |
| `vrik/motion_trials_d67.h` | Persistent mode 4, menu mapping and INI validation; separate priority over finite pacing comparisons. |
| `vrik/early_frame_pacing_d45.inl` | One pre-update pose attempt per native frame, then immediate return before pacing waits; gated receipts. |
| `xr_manager.cpp` / `.h` | Pose-only refresh receipt, try-only sampling in mode 4, source/epoch and predicted-time qualification, actual publication result. |
| `frame_sync.cpp` | Bypass the separate legacy camera token wait in mode 4 while retaining camera/Present ownership. |
| `camera_hook.cpp` | Mode-4 failure diagnostics require F2/F7; existing fallback and late head route retained. |
| `vr_menu.cpp` / `.h`, `mod.cpp`, experimental live settings | Four visible choices, correct highlight/readback, saved mode 4 and live cycling; corrected the misleading old gate-zero comment. |
| `subsystems/base/startup_setup_s15.inl` | Verify stream close and staged/final explicit graphics selection, including independent Native-0 verification. |
| Tests, current docs and build identity | Reproducible focused checks and precise current behavior; dated R1/R2 reports remain history. |

The production-connected core fixture passes **51 checks**. It executes the
actual dispatch and extracted manager functions with the real tracking
publication mailbox and controlled XR/clock boundaries. It covers both token
wait paths, mode switching, one attempt per frame, real mutex contention,
partial tracking, original-age fallback, source changes, stalled predictions,
existing D69 policy and diagnostics off/on. Independent review checked the
final source hashes and the additional tracking-family contract fixture passed.

The menu fixture passes **207 checks** using the production reducer, extracted
click/draw branches, actual hitbox definitions, getter and INI statements.
Restoring the old three-button array causes 27 expected failures; restoring
the old load clamp causes 10. These controls demonstrate that the fixture
detects losing the visible or persistent mode-4 choice.

The launcher fixture passes **47 production checks** with real temporary
config files and the actual extracted SaveSource and Save/Keep handler.
Omitting the generated mode assignment is rejected by the independent selected
mode check in four fault-injection checks. Removing final readback causes four
expected failures. Native transitions from DLSS, FSR and XeSS, stale UI state,
duplicates, missing config, Keep, explicit presets, rename faults and wrong
installed values are covered. Windows controls/file APIs use explicit host
adapters; the pending INI transaction is statically reviewed, not claimed as a
native Windows execution.

The package separately replays exact-hash check, apply, idempotent apply,
check-revert, revert, idempotent revert, edited-file conflict, new-file conflict
and corrupt-payload refusal for each overlay. These are source-delivery checks,
not game tests. Existing source files with unrelated edits are not overwritten.

## Practical limits and integration

Uncapped removes the identified game-thread token waits. A separately enabled
manual limiter, runtime/headset cadence, driver policy and actual CPU/GPU work
can still limit the result. There is no measured FPS gain or headset smoothness
claim for the new mode. There is also no new claim that DLSS/FSR image quality,
elbows, reload completion or the other R2 open gameplay issues are solved by
this timing option. Their retained repairs and limits remain in the R2 review.

For an exact R2 source tree, use `incremental_from_R2/apply_patch.py --source
"PATH_TO_R2_SOURCE" --check`, then the same command with `--apply`. Its revert
returns to R2. For the original uploaded D85R5 source, use the root helper;
its revert returns to that original source. The root `READ_ME_FIRST.txt`
contains complete commands and baseline identities. Do not apply either
source overlay to a game installation or force it over a conflicting edit.

No DLL was built, as requested. The saved menu and launcher changes become
available when this reviewed source is integrated into the normal build.
