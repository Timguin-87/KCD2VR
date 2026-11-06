# R95 runtime repairs — 4 November 2026

This is a copy-over source patch for R94R1, commit
`ec9a342aeea27e216752f96b01e19ae722734a4b`.
Build marker: `KCD2VR-R95-RUNTIME-REPAIRS-20261104`.

The latest tested DLL had the R94R1 marker and SHA-256
`1b2959748e91abb65162cc9243b11577f5cd523080447a7781ea1fcb306f5c82`.
It was a local rebuild, not the earlier delivered binary. This patch is based
on the matching reviewed source, with the captured runtime treated separately.

## What broke and what changed

### Crossbow reload ownership

The last recorded medium-crossbow reload repeatedly paused and resumed an
event-free native transition. Each of two attempts alternated speed zero/one
roughly 33 times; neither reached physical string-pull progress. Tool pose/taken
flags existed, but no stable visible-tool handover followed. The right-hand
bolt was never bound in the captured sequence. Adding another bolt transform
would not repair that earlier failure.

The event-free transition now advances at its original native rate even when
an auxiliary tool-pose packet is missing. A fresh body/owner is still required.
The presentation check accepts monotonic progression of that same transition;
the mechanism and effectful reload phases retain their exact checks.

An unchanged observation no longer invalidates the completed passenger pose.
Native metadata and effect boundaries still invalidate it. A short publication
gap retains only the actual achieved placement and its original age, at most
80 ms. Reusing it does not renew tracking or earn reload progress.

Physical Grip intent admits a new VR reload. An ordinary native LT request can
now remain native instead of being immediately captured again. A successful,
native-confirmed cocking action can admit its corresponding loading child once,
without demanding another squeeze between those two native actions. A failed
action cannot repeatedly reacquire itself. Its rejection ends at the real native
Stop; pending clock restoration is kept separate from that action lifetime.

These are shared reload-path repairs, not evidence that every crossbow has
passed in the headset. The bow's aim, arrow placement and release path were not
edited. The existing native tool/bolt attachment path is retained.

### Horse upper body and remount

Sword walk/trot aliases were absent from the seated classification. All 228
accepted mounted pose rows in this capture lacked a valid shared body frame.
The standing HMD-height policy could therefore be used with a seated pelvis.

The exact retail seated gait/idle/combat aliases now select the mounted frame.
Mounting and dismounting remain native. Once a positively identified seated
pose is present, upper-body VRIK resumes immediately; there is no extra one-second
wait. Native action calls and events continue underneath the visible upper body.

The seat reference has a separate lifetime. Dismount, remount, actor/pose-epoch
changes and loss of seated ownership retire it. A remount captures the current
native seat pelvis instead of carrying the old standing or mounted reference.
Failure rejects only the current pose and retries on the next valid one. It does
not reset calibration or permanently turn riding VRIK off. Unrelated paired,
death and unseating actions retain native ownership.

### Hand mode must not switch off holsters

The public Hands selection still requested the old exclusive boxing tuple:
pose5, weapon0. This explains the recorded transition from normal weapon handling
to missing holsters. Selecting hand/contact behavior now preserves an active
weapon owner, combat selection and weapon request epoch. Leaving the additional
hand experiment clears that experiment only. Starting Hands from OFF uses the
normal pose4/weapon6 handling path.

The combined weapons-and-hands tuple also requested incompatible pose5/weapon7:
enabling weapons changed pose5 internally, then the mismatch check switched the
whole mode off. It now uses the existing normal pose4/weapon7 combination. Saved
pose4/weapon6 is recognized by calibration restoration.

### Uncapped hand-pose starvation

The failed uncapped run recorded 7,982 stale-prediction refusals and only three
publications. Its runtime prediction horizon could exceed four 90 Hz periods.
That future forecast was incorrectly treated like the age of an old sample.

The future-horizon rejection is removed. Anchor age, source epoch, actual tracking
validity and the existing short tracking grace are unchanged. The anchor is also
read before the clock used to validate its age, preventing a newer concurrent
publication from being falsely marked stale. There is no added hand filter,
delay, camera target change or compositor scheduling change. Paced/default timing
and the user's saved timing selection remain unchanged.

### Saved corpse Grip

The old fallback nominated skeleton bones and then drove an entity root with
SetVelocity. That was not the articulated-part path which successfully moved a
dead body on the weapon.

The matching companion script now nominates native-dead entities using bounds
or world position. The DLL independently resolves the current relinquished
articulated body and a real native physics part. Grip retains a local contact
anchor and drives that part toward the hand through the existing exact-retail
part-velocity function. It does not turn a skeleton bone number into a part ID.
The unused legacy entity-root query prerequisite was removed.

Either empty hand can acquire a corpse. Shared claims exclude simultaneous
weapon/hand motors on the same corpse. Release, stale tracking, changed part or
save/actor lifetime retires the hold; a later valid squeeze can acquire again.
No acquisition snap, fabricated damage, death command or release impulse was
added. The accepted dead-body weapon impalement was not changed.

Enable **Experiments > NPCS > Physical corpse dragging** to try this opt-in route.
F9 records its diagnostics; it is not necessary to enable unrelated weapon
physics. Open an empty hand near the corpse, then squeeze Grip. The current part
origin must be within the existing 25 cm acquisition distance, so a large torso
surface can still be conservative. Bodies with no qualified articulated owner
are not forced through a guessed writer.

## Validation and limits

- Complete changed production translation units pass MSVC syntax compilation.
  This is not a Windows DLL link or an injection test.
- Production hand-mode routing passes ownership/retry checks; calibration and
  finger compatibility checks pass.
- Actual uncapped manager/dispatch functions pass 70 host checks, including the
  recorded forecast condition, concurrent publication and stale/lost tracking.
- Mounted classification, handoff and frame tests pass, including remount with
  a synthetic 2 m model-origin change. That is not a replay of the headset scene.
- Crossbow transition, publication, ownership, placement and two-child reload
  tests exercise production code with explicit native-memory doubles.
- Six corpse C++ suites pass normally and with AddressSanitizer; Lua 5.1 executes
  36 tests. The generated main PAK contains the matching new corpse protocol.
- Independent review checked the reload, riding, corpse and root changes. It
  found the native-Stop lifetime problem and the prediction-read race above;
  both were corrected before packaging.

No new recordings, DLL build, installation, INI rewrite or calibration reset was
performed. The frozen R94 source remains clean. Focused results and hashes are
included with the patch; the historical raw logs are not duplicated.

The native pickup route that was finally reported without object flicker is
preserved. This patch does not claim to repair the separate instant physical
pickup mode, intermittent mortar emptying, killing-blow impalement animation,
or all remaining contact problems. The captured killing-blow candidate had low
recorded energy and no proven piercing-tip qualification; removing native
eligibility or suppressing NPC death globally is not a repair for that evidence.

## Applying the files

1. Extract the source-files ZIP into the root of the complete R94R1 source,
   alongside `build_release.bat`, allowing these named files to replace theirs.
2. Run the existing `build_release.bat` when ready. It builds the DLL and
   regenerates the companion from this source. Keep the existing game INI.
3. Use the resulting DLL with its generated main companion PAK. The separate
   companion ZIP supplied here contains that same updated gameplay PAK if needed.
   A PAK alone cannot enable the new DLL-side corpse motor.

Optional gore content remains separate and unchanged. The source patch contains
no executable DLL, raw log corpus or replacement user settings.
