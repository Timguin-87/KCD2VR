# Current limits — R105

The current repair is based on the supplied R104 source. The supplied recordings
identify R94R1, R97 and R101; none records the supplied R104 or this R105 build.
Source defects and host regressions are documented in
[R105_REPAIR_REVIEW.md](R105_REPAIR_REVIEW.md) and
[R105_VALIDATION.json](R105_VALIDATION.json).

The Windows SDK/MSVC, game and headset are unavailable in the repair environment.
No new DLL link, SteamVR/Meta/VDXR game run, native spanner mesh inspection or GPU
measurement is claimed. The intermittent upside-down startup remains a runtime
check: R105 repairs session/reference recovery and adds first-pose diagnostics;
it does not apply a guessed headset-axis flip or alter the accepted head-follow
math. Runtime input/layer defects are independently reproduced in host tests.

The 3 m ordinary HUD plane and inward semantic safe area need a headset comfort
check. Palm gestures reveal the existing HUD on that plane; icons are not
physically attached to the wrists. Optional existing HUD rotation lag can still
move full-field effect edges. Native dialogue response choices and essential
menu/minigame controls remain outside ordinary interaction-hint suppression.

Physics/combat and the working reload state machine are preserved from R104.
The spanner change repairs only medium-tool native grip capture; actual mesh
placement and complete class 10/14/15 reloads require the game.

The older review below is retained as historical context. Its statements about
previous compilers, supplied logs or tested revisions do not describe R105.

## Historical R94R1 limits


This status refers to the supplied R93R4 source and the Repair R1 changes dated
4 November 2026. The latest substantive recordings supplied with the review are
R92R1; they are historical evidence, not a headset test of R93R4 or this patch.
The ten log files include paired main/diagnostic files, not ten independent
gameplay sessions. Settings saved at process exit do not establish live options.

See [the integration review](R94R1_REVIEW.md) and its delivery manifest for the
final source, focused MSVC checks and Windows link. There is no game run, GPU
measurement or headset validation of this successor. The original repair handoff
is retained separately as proposal provenance.

| Area | Current source status and remaining evidence |
| --- | --- |
| Accepted view | Camera-head sampling functions, stereo rendering/composition, projection and calibration are preserved. Independent hand acquisition uses a head reference in its own pose family and never publishes the camera head. Added XR lookups still require an actual performance and comfort check. |
| Fresh hands | Uncapped mode can acquire fresh hands under head-source modes 0, 1 and 2. Its bounded coherent prediction anchor, nonblocking locks, generation/epoch checks and separate publication lane have host tests. Source mode 3 remains unsupported. Final body-to-palette-to-eye smoothness and long-session behavior are unmeasured. |
| Elbows | F7 identifies the actual selected solver and records the R89 calibrated hand cues, body cue, native seed, target, prior state, confidence and clamps. Synthetic production-serializer replay is exact. This is diagnostic repair; it does not establish natural-looking elbows or replace the numerical solver. No earlier elbow configuration is declared a known-good baseline. |
| Crossbows | Auxiliary gaps and missing placement receipts have a bounded pause on the exact owned reload clock, without new gesture/event/ammunition credit. Source, target and wrist publication are joined in the committed packet. Hand ownership is side-specific; native torso/reload progression and native event accounting remain. Actual tool/bolt/string appearance, all crossbow classes and paid native reload completion need runtime confirmation. |
| Ordinary pickups | An adopted pickup retains its exact resource presentation during the native-policy transition; the exact finishing/adopted witness remains reachable when ordinary binding succeeds while the independent gameplay-policy cache still says false. The following tracking producer/body foundation now carries that same typed adopted receipt; ordinary releases do not invalidate a normally allowed body. Host checks cover source/owner replacement and timeout. Acquisition, both-hand display, real haptics, stow and throw behavior still need the game. |
| Alchemy | Snapshot acquisition precedes the validation clock, and validation uses those same snapshots. Native resource, station and ownership checks remain. All resource variants and delayed native attachment/readback still need runtime evidence. |
| Dice exit | A missing/stale policy sample is a temporary nonauthorizing gap, distinct from an explicit option OFF or a confirmed context change. Retained visit identity allows exact cleanup and cannot authorize a new edit. Actual displayed restoration requires a current game run. |
| Physical dice cup | Take, shake and place are still incomplete. The supplied bridge supports right-hand pointing plus native A input. No verified native cup occurrence, complete hand attachment/placement owner or once-only throw accounting exists in the supplied source. Those existing features must not be described as physical cup handling. |
| Small objects | Repair R1 replaces scenery teleport carry with a finite-mass native velocity drive, leaving native physics enabled for collision response and retaining achieved release motion. RigidBodyLight classification is corrected. Admission verifies original contact/body identity, one valid slot, unchanged render bounds/scale/pose and rigid-body/mass readback; it does not verify the new collision mesh. Limits are 3 kg, 65 cm bounds diameter and 30 cm grab reach; the motor releases beyond 35 cm target/body separation. Promoted scenery remains rigid after release. Physics-less meshes and fixed world brushes are not covered; collisions, material breakage and performance are runtime checks. The experiment and missing-key default stay OFF; explicit saved values remain respected. |
| Doors | Native key/manual-side unlocking and its existing hand clip remain available. Truly unusable locked or unknown-state doors cannot acquire the gesture; relocking revokes seeking. Retail Lua control flow is exercised in both load orders under a host Lua VM; retail Lua 5.1/game integration and visible swinging are not executed here. |
| Stealth overlays | Existing lockpicking/pickpocketing fitting uses a native UI parent so artwork and cursor scale together. Current adapter tests check fit, readback, bounded retry, restoration and destruction. No speculative scale change was made. Headset sizing and cursor alignment remain visual checks. |
| Combos / paired damage | Native candidate callback observation is now distinguished from an unavailable hook, including when the extra VR filter is OFF. Native recipe progression and final-input contact permission remain separate from paired damage. Masterstrike/finisher damage is still native 1x: tracked-blade authority for the exact authored event is incomplete. No 64x timing or automatic hit is enabled on pair identity alone. |
| Haptics | A producer accepting a haptic request is distinct from OpenXR submission and from a physically felt pulse. Gated receipts can establish submission/rejection, not actuator delivery. The latest-value mailbox can overwrite an accepted cue before consumption, leaving no terminal receipt; missing completion evidence does not prove delivery. |

## Retained work outside this repair

Existing DLSS/FSR/XeSS implementation, native graphics switching, map rendering,
smithing, kicks, knives, corpse dragging, roomscale/dodge, riding and dialogue
behavior are not certified by this patch. Their earlier evidence and unresolved
runtime questions remain; this update does not convert historical claims into
new test results. Optional gore remains a separate component.

All diagnostic additions use existing finite capture gates and the central log.
Hook installation, script submission, SDK success, accepted body publication and
an input handler returning are each narrower observations than a displayed eye,
acquired item, moved body, completed paid reload, delivered pulse or landed hit.
