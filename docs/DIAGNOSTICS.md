# Diagnostics

Recording is optional. Working gameplay preferences must not depend on a
diagnostic key being held or a capture lease remaining active.

| Shortcut | Evidence |
| --- | --- |
| F2 | Off → bugs → bugs + timing comparison → bugs + elbow/shoulder comparison → off. Includes sword, bow and crossbow draw lifecycle and placement. |
| Shift+F2 | Performance capture without turning on unrelated heavy interaction recording. |
| F3 | Combat recording on/off: requests, native admission, reaction/paired-action identity and results. The shortcut does not select a gameplay combat mode. |
| F5 | Off → interaction recording → physical Grip/spatial-door comparison → guided-crafting/legacy-door comparison → off. Records target, action, attachment, return, transfer, storage haptic, smith fall/LT, applied alchemy rate, native dice state and physical reload ownership. |
| Shift+F7 | Off → baseline recording → fresh-publication comparison → elbow-preference comparison → off. Includes raw goal, final wrist, contact identity/normal, freshness, release correction and physical reload ownership. Plain F7 (R124) opens/closes the VR menu. |
| F6 | not a recorder: switches Theater (flat screen) <-> Immersive. ends on the next F6 or the next native menu/scene change. an L3+R3 click only switches back to Immersive. |
| F9 | Independent finite Physics recording: corpse service, contact/raw-ray and native-held stage evidence. It does not select a gameplay physics mode. |
| Shift+F9 | Read-only upstream detail ladder under the same Physics lease. |
| F11 | Off → reload recording → staged-reload comparison → off. Includes string/tool/bolt identity, grasp and native cock/load state. |

D84 map profiling requires F2 or Shift+F2; opening the map alone does not start it.

R90: short mod status lines in the headset (the `KCD2VR ...` info text, e.g. a kick, crossbow
or door refusal) show only while F2 records. F3, F5, Shift+F7, F9 and F11 still write them to
KCD2VR.log ("VR status: ...") but never on screen. with F2 off nothing is shown.

D85 F3 combat zones: every `H29 STRIKE_RESULT` is followed by one `D85 F3 ZONE` row with
`origin` (where the swing started), `sent` (the zone our native release carried), `requested`
(+0xF0), `committed` (+0x180), `executed` (the zone the native attack row really ran, Henry
frame = symmetric(+0x150)), `rowZone` (+0x150 as stored), `inputClass` (+0x270) and a verdict.
`executedZone` in STRIKE_RESULT now means the same executed zone. Before D85 it printed +0x270,
which is the running attack's input class (1 = heavy), not a zone. COMBAT_STATE rows show
`inputClass=` and `executed=` with the same meaning. The native combo matcher compares the
executed zone with the recipe GuardZone, so this row tells you directly why a combo step did
or did not count.
The optional worker snapshot and scope clocks also stop when recording is off.
CPU scopes can be nested; do not add inclusive timings together or call them GPU
measurements.

The draw observer only reads during a short qualifying draw window. Queue overflow
and exhausted budgets are reported. No row can mean a dropped or out-of-window
observation, not necessarily that the native callback never ran. A hide call or
placement readback is not proof of the first visible pixel.

F7 records existing contact results. It does not add a second physics solver.
Recording has a cost, especially while contacts change continuously. Use it to
find a specific transition, not as a permanent performance setting.

Keep the DLL hash, active INI and companion identity with a recording. The folder
name of a later review is not the build that produced the log. Existing recordings
already cover many pickups, reloads, kicks and crafting actions; search those
before asking for the same activity again.

F3 `R3_NATIVE` (D85) also writes `opponent`, `combatObj`, `combatObjVt`,
`combatObjVf18`, `route` and `cooldownLeftMs`. combatObj is actor+450, the thing
the game asks (1315CF8) before it picks combat or free kick rows without an
opponent. One F3 session with a no-target R3 (fists up, then a sword out) tells
which object that is and if FREE_GUARD route was right.

## Finite sessions and combined recordings

The normal finite lease is 180 seconds. Stopping a key stops new observations;
previously captured records and terminal summaries may drain for up to five
seconds under their original session. A replacement session rejects older rows.
Keys can run together. Shared reload and passive-contact observations carry one
originating recorder plus the active mask, avoiding repeated native queries.

F5 samples the dice action map/cursor only; it does not move the cup. F9 provider
READY is installation evidence, not a successful grab or measured body movement.
F3 pair identity is not authority for native damage to follow the VR blade.
Read CHANGES_D84.md for the added fields and REGRESSION_LEDGER_D84.md for the
remaining owner joins. Gameplay retirement and failure-status publication remain
active with diagnostic keys off.

## One mod log

All mod-owned runtime records use KCD2VR.log: ordinary startup, companion loading,
map performance, finger-reference audits and F-key captures. The logger is
configured on the mod worker before hook startup. Capture toggles never open,
truncate or close another file. Explicit capture/map batches are admitted even
when ordinary verbosity is WARNING or ERROR; the worker restores that verbosity
after emitting the batch.

Native companion callbacks enqueue records within the logger's 512-byte inline
limit and never wait for a disk flush. Capture batches are formatted/drained on
the existing worker. Health receipts distinguish producer drops, forwarded
records, rejected batches and shared logger losses. Forwarded means admitted to
the logger, not proof of durable disk delivery after a crash.

No new KCD2VR-fingers BIN/JSON, DetourModKit_Log, map-performance or
companion-startup sidecars are created. Finger references are rebuilt from their
authored curves and cached in memory; player calibration remains in the INI.
Obsolete pre-stowed-bow and migration backups are removed from active producers.
The single last-good INI checkpoint remains a configuration recovery file. Game
or external-loader logs are outside this mod's logging policy. This source update
does not delete old files that already exist on an installation.


## D85R5R1 additions

| Capture | New evidence |
| --- | --- |
| F2 | Completed temporal pair publication, exact attempt/recording identity, per-eye reset and input metadata. DLSS allocation size is explicitly not claimed to be its active render subrectangle. |
| F3 | Per-session combo rearm evidence and explicit accepted/unknown/zero throwing-stock status. Kicks retain actual route and observed attack type. |
| F5 | Fixed-code Lua preparation result and bounded native small-pickup admission phase; Henry transfer/turn timing; crossbow mechanism observer reasons. |
| F7 | ANATOMY_D86 calibrated hand cue, head/chest axes, bow influence and destination versus filtered proxy intrusion; crossbow observer reasons. |
| F9 | Existing current-owner corpse/provider and physical contact evidence. Installation/READY alone is not a successful grab. |

Reload reasons are shared with the retained F11 capture without extra native
queries. New receipts have finite session budgets and are inactive with their
relevant capture keys off. They use KCD2VR.log; no extra runtime file is created.
Source host-test reports are development artifacts, not game runtime log files.

## D85R5R2 corrections

| Capture | Additional evidence |
| --- | --- |
| F2 or F5 | Changed presentation zoom/mode: requested, stored and XR-effective framing; separate source FOV; produced/consumed stereo identities and DLSS fault. The snapshot is non-atomic and queue submission is not headset scanout. |
| F7 | BASIS_D86 reference bones, rest/desired rotations, hand conversion, chest/hinge basis and an exact observation join to the final ARM row. These copy the existing pose transaction without new native queries. |
| F9 | Existing corpse-query receipt gains sphere-call/reuse and bounds counters plus sampled nativeQueryUs. Ordinary queries perform no added duration-clock reads. Background search receipts do not imply a deliberate grab. |
| F5, F7 or F11 | D87 RELOAD_NATIVE_DECISION joins exact clock refusal, native layer/scope, duplicate qualification fields with known-read masks, and interval/event from/to/permitted values before restoration. It uses the existing queue and native reads. |

Reload decision records have a 32-edge budget per domain/session/thread inside
the existing finite lease. Priority changes between active captures cannot
renew a previous session's quota. OFF or expired sessions do not enqueue these
records; the worker checks the lease again before formatting and writing.

The final INI is an exit snapshot. It must not be used to reconstruct the live
options during an earlier action. No capture being active, no installed hook,
and no passing host fixture by itself proves an in-game interaction completed.

## D85R5R3 Fresh Pose Uncapped

The new saved mode remains uncapped during F2/F7 comparisons. A finite capture
does not change `VrMotionTrialD67=4` back into paced mode 2. Gameplay refreshes
continue when the recording is off; the added receipts stop.

F7 adds a rate-limited `FRESH_POSE_UNCAPPED` row containing the native frame,
camera source, result/reason, publication status, locate duration, completed
sample sequence and original source clocks, actual/requested target and
valid/tracked masks. `waitNs=0` refers to the absent token wait; it is not a
claim that OpenXR lookup itself is free. `meshSubmission=UNOBSERVED` is explicit.
F2 uses the existing ownership queue with `UNCAPPED_POSE_LOCATE_NO_TOKEN_WAIT`.

Reasons distinguish a completed publication, pose/grip contention, disabled or
changed tracking, unavailable clock/session, stale prediction, rejected
publication and preserved cached/offset camera policy. A successful publication
can describe partial/invalid tracking; use the masks rather than assuming both
hands were measured. Retained snapshots keep their actual timestamps. The
existing mode-4 synchronous-camera failure receipt is also gated by F2/F7.

The new mode does not create a log file, enable capture on selection or extend
the existing finite capture leases. The launcher reports write/verification
failure in its existing dialog; it creates no diagnostic sidecar.

## R91R1 lifecycle receipts

The completed-output path, terminal DLSS refusal, one-eye reset cut and unconfirmed
native resize preserve bounded scalar status in KCD2VR.log even without F2. These
are lifecycle/fault receipts only, never screen messages or continuous profiling.
The ordinary capture writers remain gated. No extra runtime file is opened.
