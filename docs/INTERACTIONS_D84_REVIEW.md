# D84 interaction, alchemy and dice review

## What changed

This review repairs two concrete gaps in chest storage and adds focused observations where the previous logs could not distinguish a requested action from its actual native result. It preserves the newer alchemy fixes after comparing their source with D43, D46 and D63R2.

**Chest storage now produces a confirmation pulse on the hand that acquired the item.** There are two paths, and both needed the repair. The instant pickup path pulses only after its native inventory readback succeeds. The older native attachment path pulses only after the original transfer reports its checked storage result and the same hand releases that item. Failed transfers, loose drops, cancelled actions and repeated retired callbacks do not pulse. Haptics work with F5 off and still respect the user's haptics preference and XR availability.

**The staged physical pickup owner's chest gesture now follows the completed requested wrist, before collision correction.** A hand stopped by Henry's torso or a nearby surface must not make a real chest gesture impossible. The displayed wrist remains subject to collision response; moving that displayed wrist alone cannot manufacture the gesture. The legacy kinematic path can use its existing allowed pose when it has no requested-pose field. The separate native attachment owner's gesture source is retained; its repair here adds the missing verified-storage feedback.

**F5 now reports the rate actually applied to alchemy's native presentation clock**, alongside the requested slider value and reasons a qualified action kept another clock owner. It does not call a successful clock write a completed pour, ingredient transfer or audible sound.

**F5 now samples native dice state at the existing completed-frame boundary.** The snapshot reports the current native action map and cursor index. It does not move the cup, select a die, change the camera, or invent a native cup owner. F7 can run alongside it to record Henry's pose.

The core changes are in [staged pickup](../src/vrik/staged_pickup596c2.inl), [native grasp](../src/native_grasp59c13.inl), [alchemy presentation](../src/subsystems/alchemy/presentation_speed_d81.inl), and the new [dice observer](../src/subsystems/dice/diagnostic_d84.inl). Integration in `activity_input163.cpp` uses the already existing outer-frame callback. The only integration change in `action_time_owner59c11.cpp` is a global haptics header include.

## What the last 50 recordings establish

The canonical corpus contains 50 byte-distinct diagnostic files selected from 69 distinct files among 90 retrieved candidates, with the selected recording timestamps spanning October 1 through November 1. A distinct file is not necessarily a distinct play session. The Nov 1, 16:53 recording (`019c38f5f4e8`, historically labelled D79) has no detected in-file build marker; its historical build association is not an exact identity claim. The newest detected build marker within these selected 50 files is D80; these recordings do not establish a runtime result for D81, D82, D83 or this D84 source.

The interaction audit scanned all 50 original/decompressed files and classified 16,289 matching lines. Its per-file records preserve source SHA-256, archive member, original line numbers, counts and the first/last examples for each outcome. Counts describe diagnostic lines, not independent successful actions. An absent message is not evidence that a feature failed. The reproducible scanner is [audit_interaction_history_d84.py](../tools/audit_interaction_history_d84.py). Canonical selection is `evidence/log_corpus/INDEX.json`; this lane's results are `evidence/interactions_d84/history50/`.

| Recording | Evidence in the original log | Consequence for this repair |
|---|---|---|
| D80, SHA prefix `84ddd2b40829` | Line 82700 refuses instant pickup for target 525213 with `OBJECT_NOT_VERIFIED_RIGID_BODY`, body vft `3A578C0`. Line 83119 later reports the same target/key `INVENTORY_COMMITTED`, native side 0. | A refused instant path can fall back to a successful native attachment pickup. A fix limited to the instant terminal would still omit the confirmation pulse on this recorded path. Side 0 is the right hand in the current mapping. |
| Nov 1, 16:53 recording, `019c38f5f4e8` | Eleven `FIST_OWNER` refusal lines; eight native `INVENTORY_COMMITTED` lines. | Retain D80's selected-hand fist ownership check. A global fist veto can block the other hand. These rows are not a success-rate denominator. |
| D78, `dfca44453b54` | Ten `WRIST_OWNER_NOT_ADMITTED` refusal lines, nine native inventory commitment lines. | Retain later ordinary-gameplay admission for instant pickup. Do not reinstate an F5 experiment or workstation-only wrist requirement. |
| Nov 1, 16:53 recording | Six `TAKE_INGREDIENT` and six `PLACE_INGREDIENT` outcomes report `semanticReadback=1,parentRetired=1`. Two left-hand `TAKE_OBJECT`, source 5, report `semanticReadback=0,parentRetired=1` at lines 776924 and 988690. | The ingredient path was completing native semantics in this recording. The object path had an unverified effect. Current D80+ code already observes delayed attachment readback for up to 125 ms without repeating the semantic helper; keep that correction. |
| D78 | Line 440176: mortar `status=4`, `nativeStopAttempt=0`, phase 3, `phaseAgeMs=5015`, `requestAgeMs=6781`. | The old phase watchdog expired while native work still owned the current child. Current D78R2 allows that qualified working child to finish within the bounded request lifetime. |
| Nov 1, 16:53 recording | Lines 730675 and 956585: mortar `status=2`, `readyIsNotContentsTransfer=1`. | This is readiness for native placement. It does not prove contents were poured or the tools were returned. Keep the separate contents/return receipts. |
| D78 and Nov 1, 16:53 resource events | Genuine native Attach/Use/Detach callbacks return, but messages explicitly say `semanticEffect=NOT_OBSERVED`, with emitter unjoined. | A native callback return does not prove audible feedback or particles. Do not relabel these old records as evidence that sound worked. |

Earlier recordings also contain successful native drops, including eight `INLINE_NATIVE_RETURN` records with `released=1` and physical completion explicitly unobserved. That establishes native dispatch and hand release for those occurrences; it does not certify every object's later collision response.

## Ordinary pickups and small objects

The current ordinary instant adapter has a genuine native no-child route: it qualifies the current item request and invokes the typed native transfer machinery while omitting the authored pickup child. It is not a visible prop pasted to a hand while inventory state remains unrelated. Its storage terminal checks current inventory membership; the native fallback's original transfer has a separately pinned semantic result. The D84 feedback uses those two different success contracts correctly.

The adapter can acquire with either hand. The closest eligible hand must have current tracking, a fresh grip occurrence, the required reach and an uncontested ownership claim. The old global fist veto and F5-only admission must stay removed. Tests exercise the actual admission and both terminal owners, including sequential use by each side.

There is still **one global staged prop and one native pickup owner**. This supports sequential pickup with either hand, not two unrelated ordinary props held simultaneously. Alchemy's native two-tool pair is a different owner. Claiming general simultaneous two-object support would be incorrect.

The small-object admission added in D81 is also real, but narrower than “everything visible can be picked up.” It checks a known inventory item identity, bounded mass/render geometry, the current entity/body and the physical wrist. It can prepare qualified static or missing-body inventory props, including the known milk/plate cases in its retained tests, without requiring F9 to be on. It preserves genuine existing rigid bodies and rejects unknown, living or articulated body contracts. See `SmallPickupAdmissionR24` in [staged pickup](../src/vrik/staged_pickup596c2.inl) and the narrow companion preparation functions in `companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua`.

A decorative `BasicEntity` cup, plate or tankard does not gain an inventory identity merely by resembling an inventory item. The older small-object handoff proposed broader decorative-object physicalization; that proposal is not proof that the current ordinary pickup owner supports it. A general world-prop feature needs its own actual physics/entity lifetime contract and remains a boundary here.

For an acquired genuine physical prop, open-hand release and throwing use the real body's achieved motion or the native drop path. There is no invented native projectile/damage result. Objects that fail native eligibility may still use native gameplay interaction, which can retain an authored approach; **instant, camera-neutral pickup is not promised for every fallback**. This patch does not alter stereo, bow aiming, pose timing, or global camera policy.

### Collision repairs retained from D83R1

The collision implementation carried by D83R1 is preserved. At this review's final comparison, `finger_contact_h31.h`, `station_hand_contact_d75.h`, `hand_pair_contact_d39.h`, `hand_present_d61.h`, `hand_present_d62.h` and `subsystems/physics/pose_free_hand_contact.inl` remain byte-identical to that baseline. This keeps both hands' mandatory contact sweeps ahead of optional precise recovery, retains a valid current result when optional recovery exhausts its budget, and keeps the station presentation correction bounded continuously instead of dropping it abruptly above 20 mm. The measured sweep origin remains separate from the bounded display position. D81's matching requested-wrist source for initial prop acquisition and later physical driving is also retained; D84 extends that same coordinate choice to the chest gesture only.

These are specific retained corrections, not a claim of universal collision reliability. The D80 capture itself describes `PALM_PLUS_FIVE_FINGERS`, a torso proxy and `dynamicEntityCollision=QUALIFIED_QUERY_ONLY` (original line 1000). This is sparse contact coverage, not a full articulated hand mesh. Its recorded non-rigid instant-pickup refusal and fallback success identify an acquisition boundary, not a proof that arbitrary scenery should be converted into an inventory prop. The selected 50 recordings predate the retained D81 collision changes, and the separately retained D81R1 evidence belongs to the prior review. A new runtime capture is required to establish D84 contact behavior across the collision modes; no blanket collision rewrite or speculative thresholds are introduced in this lane.

## Alchemy: preserve the fixes that solved specific failures

### Presentation speed and response

D81 already moved presentation acceleration to the native `IAction + 0x5C` clock before the original controller Update. Earlier code adjusted a CAF after Update while the native parent still progressed at normal speed. That explains how a slider could visibly change settings yet leave the action delay.

The current policy speeds qualified transfer, book and resource-intro presentation, with a maximum requested rate of 8 and an authored step bound of 80 ms. A 33 ms frame therefore applies about 2.424 rather than 8. The original frame delta and unrelated heat remain unchanged. Once a physical resource, grinding or bellows owner controls the work, presentation acceleration yields to that owner. Exact leases restore their own prior rate; a foreign writer's rate is not overwritten.

D84 observes those existing decisions. `F5_ALCHEMY_PRESENTATION` includes `requestedRate`, `appliedRate`, `nativeDt`, `controllerScale`, action identity and a result. An applied rate of zero means this presentation owner performed no write; it does not assert the whole engine ran at zero or native 1x. The probe reports qualified clock decisions, not every earlier admission failure or slider value at/below one.

The direct alchemy adapter already carries separate semantic and completed-parent receipts. Successful take/place feedback is selected only after semantic readback and native parent completion. D80's attachment observation waits are bounded and observation-only. D81 extends the sound cue mapping to ordinary vessels and special ingredients. These are source-supported routes; the retrieved Nov 1, 16:53 outcome rows still explicitly leave rendered prop and sound unobserved, so this review does not claim headset audio has been verified.

### Mortar and pestle

The native game has separate `entity_left` mortar and `entity_right` pestle attachments. The current binding identifies mortar type 2 on the left and pestle type 3 on the right. They remain one native minigame action/lifecycle. This supports a real two-tool pair; it is not yet an arbitrary independent world-physics grab/drop system for either tool on either hand. Resource jars are also bound to the native left-hand resource path. Ordinary either-hand pickup does not imply every alchemy tool can freely switch sides.

The review extracted the exact D43, D46 and D63R2 source members and saved their hashes and unified differences under `evidence/interactions_d84/history_source/`. This includes `input67.h`, `mortar_contact_repair.h`, `mortar_handoff69.h`, `mortar_pour70.h`, `native_mortar69.inl`, `alchemy_tool18.h`, `alchemy_tool18.inl`, and the staged pickup owner. `mortar_pour70.h` and `alchemy_tool18.inl` are byte-identical across those historical versions and current source; the changed behavior is in the connected input/owner code.

| Historical mistake | Current protection that must remain |
|---|---|
| Freezing the pestle grip while it was still in the authored take pose | D44 waits for current working mortar phase 2/3 before capturing its grip offset. |
| Requiring a physical attachment for a logically held ingredient | D47 preserves native ingredient membership independently of a rendered attachment. |
| Letting head selection replace hand-owned selection, or retaining a stale right-hand pestle nomination | D53 keeps source/destination nomination with the relevant physical hand and mortar pair. |
| Sending a short release pulse while native pickup/selection was busy, then losing the intent | D59 retains the one local transfer intent until current native idle/recipient evidence permits it. It does not repeatedly transfer. |
| Keeping a never-acquired released request forever | D61 retires it only after newer native idle and empty-hand evidence, rather than inventing a timeout success. |
| Losing the held pair on an unavailable world-palm sample | D70 retains exact attachment membership without granting new physical work. |
| Waiting for the mortar child to vanish before invoking the native Stop that retires that child | Current `native_mortar69.inl` waits for native ready phase 2, then calls the qualified Stop once. D43, D46 and D63R2 still contain the incorrect pre-Stop child-disappearance condition. Native completion at RVA `6C3046` publishes ready while retaining the child; Stop at `A97AAA` owns its retirement. |
| Missing a return action that begins and ends between worker observations | D75 consumes the exact completed return receipt and preserves the held pair after a contents-only pot transfer. |
| Keeping an abandoned held tilt pending until much later | D78R2 withdraws that exact request before Stop entry when the current physical tilt is gone; it does not undo an entered native action or call withdrawal a successful pour. |
| Timing out a still-current phase-3 work child after five seconds | The current qualified child may continue under the bounded 15-second request lifetime. Other ownership and phase checks still apply. |

These checks explain why restoring an old mortar file wholesale would revive previously diagnosed failures. This lane changes no mortar timing or semantics speculatively.

## Dice: the native action names are misleading

The supplied `Scripts.pak` makes the dice cup nonphysical by default and gives `DiceMinigameCup:GetActions()` an empty action list. `DiceInteractor:OnUsed()` starts the table's `dice:init` dialogue. These files do not expose a script-level cup grab/shake/release operation.

The exact supplied `WHGame(8).dll` was inspected: SHA-256 `bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`, timestamp `0x6A350E20`. Disassembly and research hashes are retained under `evidence/interactions_d84/native/`.

**`dice_hold` means keep/unkeep the selected die. It does not mean hold the cup.** The native action registry and C_Dice constructor register this action to RVA `2E8FA84`. That callback checks native eligibility, fetches the selected die through `2E8B8CC`, reads its model index and held flag, and toggles the held state through `2E97F64`. Mapping grip-to-`dice_hold` as a cup feature would operate on the wrong native object.

| Native route | What the inspected code does |
|---|---|
| `dice_hold` → `2E8FA84` | Toggle the currently selected die's keep state. |
| Direction/analog actions → `2E8FBFC` | Compare projected die positions and choose a neighboring native cursor index. |
| Cursor setter `2E951BC` | Change `C_Dice + 0x4F0`, update the corresponding highlight and UI position. |
| `dice_score` → `2E91B3C`; `dice_pass` → `2E919E0` | Native score/pass handlers with availability refresh. They are not cup transforms. |
| Action-map comparison `2E92D60` | Read `C_Dice + 0x30`, including `dice_interaction` / `dice_waiting`. |

The new read-only observer verifies the current C_Dice/player/running lifecycle, exact retail qualification and four literal field-use pins before reading. It rereads the relevant identities and fields before reporting. It samples at most every 50 ms, coalesces unchanged state, spaces changes by 100 ms and emits a 500 ms heartbeat within a finite budget. F5 is checked before any native diagnostic read; F7 alone cannot activate it.

The receipt labels the cursor identity and native keep state `UNJOINED`, the VR die target `UNAVAILABLE`, and the cup owner `NONE`. The opaque native game-state pointer is reported without dereferencing it. A real cup experiment still requires its native animation/attachment owner, valid manipulation interval and shake/place/throw completion contract. Reliable physical die selection also needs the visible die-to-native-index join and an acknowledgement of the resulting keep state. This patch does not expose an experiment that merely starts an animation and calls it hand control.

## Validation and practical limits

Run from the source root, with build output outside it:

```bash
python3 tools/check_interactions_d84.py --build-dir <evidence>/interactions_d84/tests --baseline <D83R1-source>
python3 tools/test_instant_pickup_d72.py --dll <WHGame.dll> --build-dir <evidence>/interactions_d84/native_pickup_pins
python3 tools/check_dice_snapshot_d84.py --dll <WHGame.dll> --out <evidence>/interactions_d84/native/dice_snapshot_pins.json
```

| Check | Result in this lane |
|---|---|
| Literal production staged consumer | 139 checks pass in debug, optimized and address/undefined sanitizer builds. The same new fixture fails against the previous consumer as expected. |
| Complete production native-grasp owner | 64 storage/haptic checks pass, including each acquiring hand, capture off/on, failed transfer, loose drop, cancellation, XR/preference refusal, stack merge and repeated retired callbacks. |
| Actual native instant pickup adapter | 37 checks pass; 15 native function pins and seven vft targets match the supplied DLL. |
| Existing ordinary pickup admission/input/small-object consumers | Retained checks pass; small-object suite includes 26 checks and either-hand admission with F9 off. |
| Actual pre-Update alchemy receipt | 19 checks pass, including requested versus bounded applied rate, resource work ownership, stale input, foreign clock value and F5/F7 coexistence. |
| Retained complete crafting clock | 50 checks pass. |
| Actual completed-frame dice observer | 17 checks pass, including deliberately invalid pointers with F5 off, lifecycle/build refusals, bounded cadence and unchanged native fixture bytes. Four literal production pins match the supplied DLL. |

Host tests substitute native leaves and OS/heap fixtures. They execute the production consumers, but do not run WHGame, Windows SEH, the headset compositor, real collision response or XR haptic delivery. LeakSanitizer could not enumerate the executor's process tasks and was disabled; address and undefined-behavior instrumentation did run. Exact Windows compilation and runtime validation remain separate gates.

For a useful next runtime capture, ordinary pickup, chest storage, loose release and every minigame belong to F5. Combine F7 when the question is whether Henry's native pose or VRIK owns the hands, and F9 when the question is actual body/contact response. The new observations are bounded and feed the existing unified diagnostic sink. No background sidecar, new camera path, fake native result, or global animation-speed multiplier is introduced here.
