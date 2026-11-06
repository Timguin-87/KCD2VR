# R96: reload, riding and recovery repair

Date: 2026-11-04. Build marker: `KCD2VR-R96-RELOAD-RIDING-RECOVERY-20261104`.

This is a complete source revision based on the uploaded R95R1 source. It retains the newer native bolt binding, articulated corpse-part motor and seated riding work, and repairs additional defects found in their actual callers and recovery paths. Extract the full source into a new directory; no earlier patch overlay is required.

The source and focused host tests are complete deliverables. A Windows DLL has not been built or linked here, and this revision has not run in the game or a headset. The distinction matters particularly for visible crossbow flicker, final riding posture and black frames.

## Input provenance

| Input | Identification |
| --- | --- |
| Uploaded source | `source(5).zip`, SHA-256 `25a3e8bb96b3ecbddda112062fd069fbd20a67f459f6c10da45e8ac8e558170a` |
| Source's original marker | `KCD2VR-R95R1-CROSSBOW-BOLT-20261104` |
| Uploaded recordings | `Win64MasterMasterSteamPGO(1).zip`, SHA-256 `90b52ba5f3230b129b6c3a41da741b1c6f2a171b4144a2c5963dde57d2785391` |
| All 25 build-marker rows in the three supplied logs | `KCD2VR-R94R1-REVIEWED-REPAIRS-20261104` |

The recordings are evidence of earlier symptoms. They cannot certify execution of the supplied R95R1 source or this R96 repair. Previous implementation notes are retained as history, with current findings and validation in this document and `R96_VALIDATION.json`.

## Horse: native transfer, then full upper-body VRIK

Mounting, alignment and dismounting keep native body ownership. A positive seated pose starts a one-second settling interval only after the transfer clips disappear from the observed animation layers. The existing full upper-body VRIK path then takes ownership, including stationary and unarmed seated poses. Ordinary gait changes do not restart that interval; a real protected native action retains its own body ownership.

The previous seat foundation could be reused across a new visit or move with native pelvis animation. Each admitted mounted visit now captures its current model-local seat foundation. Native gait sway cannot subsequently translate the common headset/controller frame away from that seat. Actual entity movement and turning remain live. Dismounting, a changed actor or pose lifetime, a positively observed load, and ownership loss retire the old visit.

The regression explicitly remounts two metres lower using the same XR sample and requires a fresh seat foundation. Height and smoothing toggles within one visit preserve that foundation while resetting their temporal filters. This addresses both stale remount height and native sway entering the shared upper-body reference. It does not constitute a headset measurement of Henry's final posture.

Primary source: `animation_filter.cpp`, `vrik/riding_vrik_d76.h`, `vrik/tracked_frame51.h`.

## Crossbows: manual string/tool and right-hand bolt

With physical crossbow reload enabled, light, medium and heavy classes (10, 14 and 15) remain hand driven independently of the Instant master switch. The complete native cock/load-cycle tests explicitly exercise physical reload with Instant disabled. The intended flow remains a deliberate string or tool pull, acquisition of the native bolt into the right hand, and its placement on the rail. The native action still owns ammunition, cock/load effects and inventory accounting.

The reload repair addresses these concrete defects:

1. A bound native spanner or heavy lever recreated an exact-clock dependency during otherwise effect-free transitions. The same qualified transition can now advance monotonically with that tool bound. Effectful clips and mechanism observations retain their exact native timing requirements.
2. A requested first tool placement or reveal could pause native reload forever. This join now has one non-renewing 500 ms deadline. Successful placement continues manually. If the join cannot complete and the exact action remains current, its saved native rate is restored. Waiting for the player to choose a deliberate gesture is not timed out.
3. The first bolt placement after native Ammo had the same indefinite wait. It now has bounded recovery within the same action, or restores native transport when that requested join expires.
4. Releasing the grip before a first bolt placement could later replay as a rail nock. A valid early release now retires that pending grasp. Recovery requires a real regrip and release.
5. Tracking acquired after the caller's time sample could appear to come from the future and revoke an otherwise current reload. Validation now samples time after the snapshot it validates.
6. The medium spanner's achieved-grip receipt omitted body scale. Its native grip conversion now agrees with the actual placement across scales and rotations. The native asset grip is preserved rather than recaptured from an animated descriptor.
7. A brief heavy-tool passenger/mechanism publication gap withdrew an unchanged achieved tool, allowing native and VR placement to alternate. The unchanged item can now retain its already achieved placement for the original 80 ms receipt lifetime. This performs no new write and does not renew a timestamp. A changed native tool, owner, attachment, pose, mount state or placement permission still ends that retention.
8. The primary crossbow's retained-placement reader also sampled time before copying its receipt. Unlike hand/tool placement, that primary callback has no shared writer mutex. The receipt is now copied before its validation time is read, removing another possible false-future withdrawal without renewing its original age.

The timeout is a recovery path for a missing native attachment/join, not the normal reload controller. It prevents a failed join from freezing the native action indefinitely. It cannot safely restore a different action through the previous action's pointer. Actual visual placement, game attachment scheduling and every individual weapon still require a current game/HMD run.

Primary source: `subsystems/ranged_reload/crossbow_native_xr34.h`, `crossbow_native_xr34.inl`, `crossbow_rifle_request_h21.inl`, `reload_passenger_xr34.inl`.

## Recovering control after failure or loading a save

Several unrelated systems had process-long fault flags or recovery checks behind the same gate that disabled their service. This revision repairs specific occurrences and requires fresh ownership after recovery. It does not restart an old held input or pretend that an unfinished native cleanup succeeded.

### Script transport, holsters and interaction clients

The shared native script bridge previously disabled itself on a fault or VM change before it could observe a qualified replacement VM. Administrative shutdown is now separate from callback revocation and a fault in one VM. Service can observe a new qualified VM or a completed positively observed load and admit fresh work. An uncertain read can also recover when the original qualified callback sees the exact stack state restored.

Entered calls are never replayed. Native stack restoration still belongs to the exact VM and stack that entered the call. An unknown UI result, a menu recheck or a setting change does not manufacture a completed load. This repairs a shared transport failure that could disable several script clients together; it does not prove every holster failure had that cause.

### Body/VRIK restoration

A failed native pose restore now records the exact failed joints, their original native values, and the current character, skeleton, buffer and player lifetime. The qualified native skinning callback checks this recovery before the ordinary VRIK and attachment gates can return because of the fault.

Exact native readback or a positively qualified new player lifetime can resolve the obligation. A completed observed save/load additionally permits one freshly qualified restoration attempt on the identical current native buffer. Only the failed joints are written, followed by exact readback and validation. Failure retains the obligation until another real load or valid native recovery; there is no retry every frame.

There is a visible tradeoff: the retained older native joint values may briefly appear on that repair callback. If the indebted buffer never returns and the player lifetime is never positively replaced, the system cannot declare its restoration complete. A rotating pose-buffer address alone is not evidence that the old native buffer has been retired. The finisher's separate failed-overlay latch also remains separate from ordinary VRIK recovery.

### Corpse movement with the hands

Either hand can acquire a qualified articulated corpse part; both hands are not required. Existing per-body/part ownership still prevents two competing motors from writing the same part. The R95 hand-relative part motor and local grab point are retained.

The duplicated permanent dragging fault flag has been removed. Native Action/status faults stop the affected capability for the failed load; completion of a real observed load permits fresh body/part qualification. An exception reported late from an older load cannot poison the newer one. An ordinary native refusal does not become a permanent process-wide failure.

A save load retires old held and pending corpse inputs even when the engine reuses the same addresses. A fresh squeeze can acquire again; the old hold is not replayed into the loaded world. Snapshot/time ordering was also corrected so a newly published tracking, grip or completed-hand packet is not mistaken for future input and dropped. Release does not add a final velocity write.

### Pending killing-blow impale requests

An already admitted lethal impale request could disappear during a same-owner hand/weapon rejoin. Its pending-state helper still imposed a 1,700 ms limit although the existing H18 weapon wait allowed 4,700 ms. The helper now uses that shared H18 deadline. It preserves the original start time and all tracking, weapon, target and contact requirements. Rejoining does not extend the request, grant a physics write or turn an ineligible contact into an impale. This fixes a separate lost-request path, not the historical blade-contact refusal described below.

Primary source: `core/observed_load_recovery_r96.h`, `subsystems/physics/load_fault_recovery_r96.h`, `native_script_t2.cpp`, `vrik/coherent_pose.inl`, `vrik/pose_restore_debt_r96.h`, `subsystems/gore/native_gore.inl`, `native_part_anchor.inl`, `vrik/lab58.inl`, `vrik/impale_pending59.h`, and `subsystems/physics/saved_corpse_drag_d72.inl`.

## Pickups, mortar, doors and held weapons

The interaction repair corrects snapshot/time ordering in instant and staged pickup, mortar, door and held-item paths. A newly published valid observation must be compared with a time sampled after that observation was copied.

An existing prop hold can survive a brief publication gap within its original 80 ms lifetime without applying force, stowing it or renewing its presentation. A real load retires the old item claim. Prop native failures are scoped to the failed load; the lab prop's three-failure rule counts consecutive failures rather than accumulating unrelated failures forever.

The mortar keeps an unspent stop request through a bounded observation gap. Once native Stop has entered, it is not retried. Fresh observation is needed to confirm progress. This targets an intermittent cancellation defect without substituting another crafting action.

Door handling remains symmetric for left and right hands. The repair removes a shared time-of-read defect in service, currentness and press admission while retaining the original 100 ms press deadline. Native key, lock, use and accounting behavior is preserved. Current retail `Scripts.pak` was not supplied, so a fresh test of the actual game's AnimDoor Lua could not be run.

Changing weapon control mode now actively queues retirement of the old held collider. It does so before a contended receipt can cause the attachment writer to return busy. Collider cleanup still precedes a different attachment writer taking ownership.

The native physics phase can admit fresh leases after an observed world reset/replacement. The weapon-body client can reopen after an already known body and geometry complete all same-generation cleanup successfully. Failed cleanup, an unknown factory result, uncertain self-pair removal, an external constraint pin or unresolved old-world geometry remain outstanding obligations. Clearing the generic phase flag is not claimed to resolve those client-specific cases.

Two other persistent cases remain unresolved: native-grasp quarantine retains unfinished exact native cleanup or speed restoration, and H40 retains an entered paused-door/rest obligation when the actual Lua VM is replaced. A replacement VM cannot acknowledge the old native clip. Loading alone does not establish that this cleanup completed, so the generic script recovery must not be read as fixing every interaction lockout.

## Findings that are not closed by this revision

### Killing-blow impalement

The supplied historical log contains a native accepted lethal thrust for hit serial 11, target 45102, at 18:06:59.728. Its receipt records feature 2 (`Blade`), and the impale request later refuses `NO_PROVEN_PIERCING_TIP`. The C44 row also reports contact energy 0.02 against the configured requirement 0.30; that field is separate from the native normalized-strength value 0.083. Native death did occur; absence of a kill is not the explanation for that refusal.

The source trace shows that gesture direction and contact feature are separate fields; the queued hit keeps its selected contact feature unchanged. The sparse shaft sweep can select a blade obstruction before a tip probe. The historical recording does not establish that a simultaneous tip contact existed, so a thrust label alone cannot justify changing that receipt into a piercing-tip hit.

This revision has not replaced native death animation or declared all lethal thrusts eligible for impalement. Corpse-hand recovery and movement are separate from proving a killing-blow impale transition.

### Black frames

The three finite recordings contain 52,244 recorded end-frame returns, all with result 0. Nineteen in `KCD2VR2.log` submitted zero layers between 17:50:51.899 and 17:50:52.240, immediately after stereo startup at 17:50:51.864. That establishes a startup interval with no submitted scene layers. It does not establish the cause of all later black frames: a successful submission with positive layers can still contain black pixels, and an end-frame return is not headset scanout evidence.

No new renderer fallback was introduced. Accepting a partly completed stereo pair would risk reintroducing an earlier rejected rendering defect. Camera, stereo and upscaler implementations were not rewritten as part of these interaction repairs.

### Arbitrary static scenery and runtime appearance

Existing native inventory-item pickup preparation is retained. These changes do not turn every static scenery object into a valid dynamic inventory item. Actual in-game pickup reach, final spanner/bolt visibility, torso orientation, holster behavior across every context and finite ammunition outcomes remain runtime verification items.

## Configuration and installation

Keep the installed INI and calibration. The uploaded exit snapshot has physical crossbow reload and riding VRIK enabled, while Instant Activities, Instant Interactions and Instant Crafting are disabled. That is an exit snapshot, not proof of the settings used earlier in a recording. Physical crossbow reload remains independent of that Instant master. The retired `VrCrossbowHandOnlyR89` compatibility field is not its admission switch.

On Windows, install Visual Studio 2022 C++ desktop tools, CMake and Python 3, then run `build_release.bat` from the new `KCD2VR_SOURCE` directory. The first CMake configuration can fetch the pinned OpenXR SDK unless an existing SDK source tree is provided as described in `BUILDING.md`. Build output is `dist/prerelease`; building does not install it. Use the generated binary and matching main companion PAK together. The build regenerates the companion from the included gameplay donor and current overlays. Optional gore assets remain a separate package. This source archive is not an installable DLL update.

Verify the new log marker is `KCD2VR-R96-RELOAD-RIDING-RECOVERY-20261104`. The source archive includes a per-file SHA-256 manifest and an exact local source-commit identifier.

## Validation scope

`R96_VALIDATION.json` contains the final host results, commands, source hashes, negative controls and input provenance. Tests execute production functions or literal extracted production boundaries, with declared native memory/API, scheduling, pose and inventory leaves supplied by fixtures.

The crossbow group has 11 final suites, 28 positive executions and 39 strict negative controls. Each negative control must compile and then fail with its expected assertion marker; compiler errors and arbitrary crashes are not counted as success. The 4,974 displayed checks include repeated build configurations and are not 4,974 distinct gameplay scenarios. After the final primary-placement repair, the four suites that compile that function were rerun. The final 128-entry source audit has 122 exact whole-file matches and six verified unchanged compiled extractions, with no unresolved mismatch; old whole-file receipts were not relabelled as fresh executions.

Riding validation covers the real animation-filter adapter, 28 lifecycle checks, 210 seat-foundation checks, 27 literal restoration/outer-entry checks, existing riding/frame regressions and 44 actual finisher checks. Several older fixtures reuse assertions. The original supplied source fails the added lifecycle, foundation and restoration cases.

The corpse group has ten passing runners, including the actual native Action/status adapters, saved and direct corpse services, same-address load recovery, either-hand input, actual physics phase dispatch and body/part ownership.

The interaction report has 26 passing entries: seven production adapter suites at O0/O2, five retained regression suites, six compiled production mutations rejected by their expected assertions, and one source-order integration check. Its 198 source dependency hashes match the final code. A separate retained static-prop group has four passing runs. Actual current-retail door Lua is excluded from these claims.

The native weapon cleanup report has nine focused cases and six existing lifecycle/install processes passing; the identical fixture compiled against the original client fails four cases as expected. Primary crossbow retention passes 13 checks, with the immediate-before function failing the one concurrent-publication regression.

The new impale pending/rejoin regression executes the actual physical weapon step and passes 152 checks in Debug, optimized and ASan/UBSan configurations. Its existing 27-check regression also passes. The verbatim original header compiles and fails the expected late-rejoin assertion. The first LeakSanitizer attempt was unavailable because the managed environment could not inspect process tasks; that failed attempt is preserved. The successful ASan/UBSan rerun disables leak detection only, and no leak-check success is claimed.

The existing giant `pose_adapter_test.cpp` fixture is stale and produced 287 compile diagnostics. It was not weakened to manufacture a full-solver pass. Focused tests compile the changed restoration and entry boundaries instead. This is not a complete Windows translation-unit build, real Windows SEH execution, engine scheduling run, renderer visibility check or headset test.
