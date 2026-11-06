# R95R1: right-hand crossbow bolt

Apply these replacement files to R95, commit bbf520d238e6b93df0ba85f46508bfc9b0d9f59d. Build marker: KCD2VR-R95R1-CROSSBOW-BOLT-20261104. This is a source-only crossbow correction. No DLL, PAK, installation, saved settings or calibration changes.

## What was actually wrong

The latest failed recording stops at normalized reload time 0.0099, just before the native LoadReachedAmmo event at 0.0100. All 338 recorded binding rows have boltKey=0. The bolt was never created during that VR-owned interval. It would be wrong to describe that entire failure as an invisible bolt or an incorrect grip offset.

The working bow and crossbow did not accept the same quiver reach. Bow retrieval uses a yaw-relative region behind the right shoulder; the crossbow used a smaller sphere. A hand 30 cm right and 40 cm behind the head is inside the bow region and outside that sphere. Crossbow loading also required a fresh neutral/regrip in its own child action. A held reach could be discarded across the native cock/load transition, or while the auxiliary pose publication was missing.

The native recordings show the real path: LoadReachedAmmo creates the right-hand entity; LoadReachedWeapon removes it from the hand and transfers ammunition to the loaded crossbow. On November 3 at 23:10:08.636, native fallback created right-hand entity 1312611, then nocked it at 23:10:09.592. That was native fallback success, not evidence that VR hand-follow worked. The older October 29 recordings contain the same complete sequence. The native factory is also the one used by manual bow retrieval; no invented SpawnArrow callback or second ammunition factory call is necessary.

## Changes

1. The crossbow RIGHT-hand quiver region now matches the working bow. Bow code itself is unchanged. Hold Grip and reach behind the right shoulder; loading no longer demands an extra release in the load child.
2. Fresh raw quiver input is observed separately from the completed weapon pose. A reach can survive the short publication gap while Grip stays held. It does not move the native clock during that gap. A newer release overrides older completed input, and old intent expires or is retired with its owner. This adds no hand smoothing, delay or frozen tracking.
3. A native-created bolt whose hand attachment is still completing is represented as pending, rather than treated as total ownership loss. The exact action, entity ID, entity and right-hand identity remain checked. Pending grants no placement or nocking permission. Its original 80 ms completion window cannot be refreshed; a complete binding cannot become pending again.
4. Once the SAME native bolt has been placed at the right wrist and read back, the existing qualified bow visibility helper restores its attachment/entity visibility. Later native hide refreshes are checked again. The bolt still uses its existing slt_0 grip helper. No duplicate visual bolt is spawned.
5. Acquisition and loading remain separate. Opening the hand away from the rail does not load it; regripping and placing the same bolt at the rail uses the existing native load/transfer path exactly once. Light, medium and heavy crossbows share this correction. String/spanner motion and their native cocking sequence are retained from R95.

Only the four ranged reload implementation files and build marker change in production. Bow aim, arrow handling, camera, stereo, DLSS/FSR, melee, pickups, riding and PAKs are unchanged. Exact retail qualification remains WHGame timestamp 0x6A350E20 and SHA256 bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7.

## Checks and limits

- Native adapter/event-loop fixture: 86 checks in MSVC Debug and optimized builds; all three crossbow classes, held-Grip acquisition, reach during a pose gap, release precedence, expiry, owner reset, no duplicate Ammo/Load event, misplaced release and rail placement. Removing raw input makes the regression check detect the old failure.
- Actual bolt binding/placement implementation: 37 checks in Debug and optimized builds. Old pending rejection, omitted visibility and detached-bolt reuse are all detected by mutation checks.
- Actual passenger consumer and quiver region: 65 checks in Debug and optimized builds; pending-to-bound transition, exact identities and yaw-relative boundaries. Both old behaviors are detected by mutation checks.
- Existing reload pipeline: 273 checks; native clock cycles: 126; transition suite passed. Existing socket/mechanism suite: 310 checks in Debug and optimized builds, recorded before the final tracking timestamp read-order cleanup; those socket/mechanism paths are unchanged by that cleanup.
- Complete production frik_pose_probe.cpp and action_time_owner59c11.cpp pass MSVC /Zs on the final source. No DLL was linked.
- The legacy run_v66108_checks.sh was attempted but could not configure because clang++ is unavailable in this Windows shell. It is not recorded as a pass or a gameplay failure. The focused MSVC checks above did run.
- Independent review found a newer-release/older-completed-input conflict during implementation. It was corrected and added to the 86-check regression case; no outstanding blocking review finding remains.

These are source and host-fixture results. Native memory, inventory effects, scheduling and headset display are fixture leaves in the host checks. No new recording was requested. This fixes concrete missing links; it does not claim a new headset-proven result for every crossbow or every native fallback condition.

## Existing evidence

Failed capture: D:/KCD2VR-review-builds/R94R1_RELOAD_RIDING_REVIEW_20261104_1819/KCD2VR.log, SHA256 7cf621f6f37d88b89579e079b7e8a2c98014504af2b347db1f53073ed8ff4708. Relevant original rows 322154/322161 and 323476/323485; bounded interval 318500-326000 contains no native Ammo/HAND_END acquisition.

Successful native reference: D:/KCD2VR-review-builds/R92R1_FOLLOWUP_20261103_2329/INPUT_CAPTURE_LOCAL_ONLY.zip, KCD2VR.log member SHA256 01c50a3f7f4f557dc5d71c60711d8fab62eb5c83f77da391f60af3afc2340dc5. Rows 293970 (VR fallback), 293975-293978 (native acquire), 294109-294115 (native remove/transfer/nock), 294123 (finalize). Earlier full chains: D46_HEADSET_HOLSTER_INVESTIGATION_20261029/reload_deep/CROSSBOW_RUNTIME_PROOF.txt and NATIVE_EVENTS_SESSION2.json. Raw logs are not duplicated in this patch.
