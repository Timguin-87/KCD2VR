# D84R1 masterplan integration — 2 November 2026

Baseline: the supplied D83R1 reviewed source. This is a selective successor, not a wholesale D86 replacement. Source and static tests establish the changes below; there is no headset result for this build yet.

## Integrated

- D84 reviewed cumulative patch: smithing acknowledgement/release correction, pickup/stow feedback, paired-callback reader correction, native kick-request acknowledgement, coherent dodge input snapshot, corpse lifecycle and diagnostic separation, compact HUD ownership changes and recording improvements. The accompanying D84 reviews describe their boundaries.
- Optional blade-zone refinement: a refused refinement no longer cancels the original preparation. An accepted change is carried into release only for the same native owner, generation, body epoch, request and gesture. The three blade-zone options remain OFF by default. KCD2 still validates attacks and damage.
- Instant-pickup hook preparation follows the saved gameplay setting, independently of F5. Enabling F5 later adds its other observation groups without trying to revalidate already-patched pickup entry points. Turning F5 off retains gameplay coverage.
- Saved crossbow reload enablement no longer depends on the Instant Reloads or F11 switch. Existing comparison/rifle diagnostics are retained; this is not a claim that every function key is now recording-only.
- K42 owned-hook validation uses the actual installed jump length. The previous 16-byte witness could overlap a neighbouring hook and incorrectly refuse the native knife pin. LEFT CHEST KNIVES (TRIAL) is restored as an opt-in saved setting, with the existing finite inventory path. No free inventory or refill was added. Actual drawing/throwing still needs live confirmation.
- D86 smithing contribution: LT-assisted sideways placement follows the native two-axis workpiece map, including axes/horseshoes. Ordinary one-axis pieces retain their previous handling.
- Windows compile repairs: missing diagnostic declaration include; avoid the Windows `near` macro in the combat helper name.

## D86 kicks and excluded changes

The supplied D86 archive SHA-256 is c434e2469f59f2595adf90066bfd09f3015dc6fbbfd36bb76db97c2cbd8e393f.
Its procedural fallback builds a fresh type-4 melee-hit request and applies native stamina effort after a generated foot contact. Native constructors/resolution alone do not prove this is an eligible native kick when the original kick was never accepted. That fallback is not imported. The reviewed native kick-request correction is retained, but armed/no-target kicks are NOT declared fixed.
D86 automatic post-minigame camera turning, new holster gates and always-visible refusal messages are also excluded. Camera/stereo ownership, bow aiming and existing anatomical solve are not redesigned here.

## Masterplan status / remaining work

1. Gameplay hooks and diagnostic separation: concrete pickup/reload fixes integrated; some legacy F2/F7/F11 comparison modes remain. Saved options and diagnostic activation are not interchangeable.
2. Interactions/props/corpses: lifecycle repairs integrated; universal small-prop pickup, stable corpse dragging and all contact conditions are not proven by this delivery.
3. Contact-first combat: optional blade-zone preservation integrated. Actual committed native collision zone, universal kicks and all combo/finisher alignment remain unresolved. No guessed collision writes or fabricated native outcomes.
4. Crafting/reload: smithing two-axis assist and acknowledgement fixes integrated. Continuous hand-driven reload, carafe pouring, completely immediate crafting and native release sound/particle coverage remain incomplete.
5. Roomscale/dodges/targeting: retain reviewed D84 routing changes; no new camera yaw correction. Headset validation remains outstanding.
6. Physical dice cup: not implemented here.
7. Knives: concrete owned-hook defect repaired and trial restored; inventory/spawn/throw success must not be inferred from passing host tests.
8. DLSS/FSR/XeSS: no new live resource-recreation implementation in this patch. Restart-free switching remains an outstanding task.

## Verification

21 focused Release tests passed with MSVC, including actual extracted pickup-hook installer policy, blade refusal/stale-owner cases, owned bow hook lifecycle, smith transitions and D84 HUD/capture tests. Real Windows x64 Release linking was run. This does not establish injection or headset correctness. The older run_v66108_checks.sh was attempted but could not configure because clang++ is not installed; no sanitizer result is claimed.
The supplied latest unified log identifies D81R1. It is historical evidence, not a test of this successor. No further headset recording was requested.

## Preserved / deployment

No game installation, user INI/calibration reset or frozen source modification. Gore is separate. Use the patch only on an exact supported source baseline; its installer validates every changed preimage, backs up and replaces files atomically. The included diff ignores newline-only differences for review. Do not layer it blindly onto D86.
