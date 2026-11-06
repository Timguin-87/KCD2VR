# R93R3: cumulative publication and handover corrections

This is a source-files delivery, with no new DLL or generated PAK. Copy the
contents of `KCD2VR_SOURCE` into the complete R93R2 source directory, replacing
matching files. The package includes every changed source dependency since
R92R1 (`22c60c4e8fa890385683b7b269f2df6d1f3cbbe2`), so the earlier R92R2/R3/R4
and R93/R93R1/R93R2 corrections are included. The manifest identifies the final
commit and every replacement. The archive is not a complete standalone tree.
Do not copy the sample INI into the installed game; existing settings and
calibration were not reset.

## What was wrong and what changed

### Bow draw and arrow presentation

The body packet was published before the separate physical-weapon packet.
A native draw callback could read body N and weapon N-1, reject that combination
and return presentation to the native animation even with valid tracking.
The reverse mismatch was possible too. The completed body now carries its own
small bow draw receipt: item identity, physical weapon transform, draw fraction
and the same pose timestamp. Draw presentation reads that receipt with the
wrists, rather than racing a second mailbox. The 80 ms freshness limit, real
tracking loss, explicit release and equipment-change checks remain.

There is no extra smoothing or frozen-hand timeout. Bow aiming, authored grip
geometry, native charge/firing and arrow transforms were not retuned. The
existing native-world agreement check still applies. The actual old consumer
fails the mixed-publication replay; the corrected consumer passes it.

### Crossbow string, bolt and tool

Reload source and completed wrists now publish together. The native reload
consumer cannot join a new source to an older body. A single retry handles a
publication serial changing while the exact native clip, clock, binding and
working-hand identity stay unchanged; that receipt also reaches the final
ownership check. Missing auxiliary reload data no longer directly invalidates
the whole body packet.

This retains the earlier right-hand bolt/tool, heavy lever and reload publisher
repairs. Light, medium and heavy crossbows share the corrected path. Native
ammunition, bolt creation, reload events and loaded state remain native.
A real native-clock advance still requires a matching pose; old joints are not
relabeled as a new clock. All crossbows completing in-headset without any flash
is not established by source tests alone.

### Pickups and held objects

When native Picking finishes, the same attached item is adopted as a held
object. That clears the old action pointer while preserving the hand, token,
item and canonical grip. If this happened during attachment placement, an exact
pointer comparison rejected the placement and allowed a native frame.

Placement now accepts that one-way transition only for the unchanged held
resource and identical canonical grip. It uses the final binding, without
reading the retired action. Changed items, hands, actions, grips or revoked
ownership still fail. Native selection, inventory accounting and use semantics
are unchanged. The earlier first-attachment and finishing-presentation fixes
are included, not replaced.

### Impalement

`VrImpaleHang`, shown as **Physical impalements** on **Immersion and Controls >
Page 2**, is the
configuration switch for this path. The producer and exact-part servo no longer
depend on F9, the separate physical-combat experiment or the retired corpse
option. Turning it off revokes the pending ownership. Native contact, current
identity, eligible physical part, death state where required, tracking and
version qualification remain necessary. This does not make living NPC grabbing
or general physics-lab experiments active.

### Optional gore: head-only caps and limb handover

The headgear policy previously admitted helmets but excluded cap assets.
Native hiding and the optional Lua presentation now agree on the verified
`/clothing/head/cap/` family. Coifs, hoods, cowls and other neck-covering pieces
remain excluded. This is not a claim that every possible headgear asset works.

A native limb cut previously treated successful script submission as accepted
ownership, even if the script was absent, incompatible or rejected the event.
The current optional script must acknowledge the exact native cut batch.
Rejection rolls back that batch. After acceptance, Lua owns durable cleanup
before optional cap/flying-piece presentation, so a later visual error cannot
cause a conflicting native rollback.

The active script is a narrow overlay on the exact D77 62.39 asset package.
The source ZIP contains the script and provenance, not its large asset PAK.
The build helper overlays only that Lua member and outputs a separate
`OptionalGore` package. Main gameplay assets stay separate. To build with those
existing optional assets, use:

```bat
build_release.bat -OptionalGoreSource "E:\backup\KCD2VR\gore_D77_fix_v62_39"
```

The helper verifies the reviewed donor; it does not accept arbitrary packages.
Its PAK SHA-256 is `5a4e6746278e145f934276a87aa29e85a8231ed9f39491d4bce29de7bdb36215`.
No new optional PAK or installed-package identity is claimed by this delivery.

## Existing recording evidence

The review indexed the 20 newest available unique main/diagnostic log files
located in the preserved archives, sorted by embedded recording start and
SHA-256 deduplicated. A main log and diagnostic log can be from one playthrough;
this is not a claim of 20 separate recent headset sessions. Missing raw sessions
were supplemented by existing indexed extracts, not reconstructed recordings.

The latest crossbow capture has valid input followed by INPUT_LOST 30–63 ms
later, before tool spawn. Older logs also contain disabled-mode, missing-primary
and stale-physics failures; those are distinct causes. Bow draw rows show native
and VR fractions diverging. Pickup evidence includes 6–21 ms attachment-to-first
placement intervals and the Picking-to-held transition. These logs locate the
boundaries; the source-connected tests establish the specific races corrected.

## Validation and limits

- Bow: 28 checks each in debug, optimized and MSVC AddressSanitizer runs; old
  source fails four intended negative-control cases.
- Crossbow: 75 debug plus 75 optimized body/source cases, 273 pipeline checks,
  111 reload-cycle checks, transition adapter and 11 publication cases.
- Pickup: 26 debug plus 26 optimized adoption cases; exact old adapter fails
  the intended transition. Eight retained targets total 443 checks, with five
  mutation controls.
- Impalement: 319 actual producer/dispatch cases and 562 composed cases, each
  passing normally and with AddressSanitizer, plus eight retained suites.
- Gore: native event/rollback, cleanup, Lua acknowledgment, eligibility, cap
  policy and synthetic optional-packaging checks passed.
- Actual `frik_pose_probe.cpp`, `mod.cpp` and `vr_menu.cpp` passed MSVC syntax
  checks. This is not a Windows DLL link or an injection test.

The requested legacy `run_v66108_checks.sh` runner stopped at configuration
because `clang++` is not installed on this host. It did not run its test suite.
The old D54 gore presentation harness has 21 failures/errors and 15 passes with
both the unchanged D77 donor and this script. Those identical results include
missing historical assets and changed donor expectations; they are recorded as
unresolved historical coverage, not silently counted as passing current tests.

No headset run, installation, camera/stereo/upscaler rewrite or bow aiming
retune was performed. Native-clock changes, unsupported headgear and native
limb eligibility refusals still have their real requirements. This delivery
corrects demonstrated source races; it does not certify every reported visual
artifact as gone.
