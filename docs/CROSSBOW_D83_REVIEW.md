# Crossbow and bow review of Claude D83

## Decision

The reviewed merge keeps the D82 native-clock crossbow owner and incorporates
Claude's useful right-shoulder quiver arbitration. It does not adopt the second
manual reload owner in Claude's replacement `staged_reload596c2.inl`.

This distinction matters because the two packages use the same D82 label for
different implementations. Claude's 24-file D83 overlay is based on D81R1; it
does not contain the reviewed D82 clock/passenger scope repairs. Applying it
alone to D81R1 therefore leaves those gates unchanged.

The resulting source still needs a Windows build and headset validation. The
reviewed D82 tests and the additional checks here exercise production bodies
with controlled native memory, input and API leaves.

## Confirmed regressions in the supplied overlay

### A comment disables the working bow's release handoff

In Claude's `ManualBowTick596`, the line setting `m.nockRequested` acquires a
trailing comment. The following `m.nockRelease56=...` assignment is on that same
line and becomes part of the comment. Native nocking can succeed without
publishing the release identity used by the existing bow handoff.

A fixture executing the literal nock-commit branch reproduces it: the native
nock leaf succeeds, while the release acknowledgement remains false for both
the ordinary bow and the proposed crossbow branch. Restoring only the swallowed
statement restores both acknowledgements in the same fixture. This is a
behavioural check of the actual branch, rather than a test for a particular
comment string.

The reviewed merge retains the existing bow file byte-for-byte from D82,
including the release acknowledgement. No bow aiming, hold, placement or native
ammunition-transfer function is changed.

### The new light-string gesture is an immediate cocking trigger

Claude's `CrossbowSpanTickD82` measures displacement with
`abs(dot(delta, railAxis))`. With the same initial grasp and a 10 cm rail, both
an 8 cm pull along the rail and an 8 cm movement in the opposite direction call
the native cocking leaf. The literal function reproduces both calls.

The function does not drive a partial native reload clock or publish a string
contact constraint while the hand moves. `CrossbowHandOwnsD82` then prevents
the existing XR34 gesture request from entering its controlled light-string or
cocked-bolt path. The README also explicitly leaves tool cocking to native
animation. This does not implement the user's requested continuous string/tool
control for every crossbow type.

Fixing the sign alone would leave a second owner and the missing continuous
clock relationship. The reviewed merge retains XR34's directed stroke,
same-owner hand constraints, native events and heavy-tool mechanism instead.

## The baseline already has a hand-bolt path

Claude's statement that the bow's quiver code excluded crossbows is true of
that particular bow loader. It does not establish that crossbows lack their
own hand-bolt implementation. The reviewed source connects the following path:

1. `CrossbowGripRequestD72` observes a qualified right-shoulder grip and requests
   the game's real reload action. The request carries the exact input owner.
2. In `crossbow_native_xr34.h`, the loading phase holds the native clock before
   `LoadReachedAmmo` until the quiver acquisition is accepted. The original
   native event creates the real ammunition entity.
3. `RangedNativeBindingXR34` and `RangedBoundHandIdentityXR34` validate the
   parent's bolt ID at `+0x88`, the current player model and the actual hand
   attachment. An arbitrary entity or another reload's bolt cannot qualify.
4. `CrossbowAttachmentXR34` observes that binding. `BuildCrossbowPosesXR34`
   uses the committed tracked wrist and preserves the captured grip as the
   native reload animation progresses. The same native entity is placed with
   its `slt_0` helper at the wrist; no duplicate visual is created.
5. The achieved entity transform and orientation are read back. Placement at
   the crossbow rail plus release allows `LoadReachedWeapon`. A misplaced
   release retains the same bolt for regripping without granting the load.

Both the attachment observer and pose builder are called by the actual shared
attachment/coherent-pose flow. The D82 scope repair connects that path to the
authored `6+2` upper-body and `0+3` full-body profiles before its first tracked
reload pose. The older one-layer assumption blocked that initial admission.

This source trace and the socket tests establish that the path is implemented
and connected. They do not establish that a particular bolt rendered correctly
in the headset; that remains part of the runtime acceptance check.

## Accepted improvement: reserve the right shoulder for bolts

The supplied D81R1 log shows right-back polearm attempts during the crossbow
session. At 00:36:02.263 it displays the polearm message asking the player to
stow the weapon; later attempts reach the holster volume and are refused by
the native-hand-key gate. Those records do not show a successful polearm draw,
but they do show an unwanted holster interpretation in the quiver region.

The reviewed merge extends the existing bow/quiver draw arbitration to a held
crossbow when any of the existing physical-reload entry modes is enabled:
Instant Reloads, F11 rung 2, or the explicit crossbow reload preference. This
matches `CrossbowReloadEnabledD65C`, instead of checking only one preference.

Only right-shoulder draw/switch candidates are excluded. Belt draws and the
left hand retain the existing policy. Stowed crossbows reserve nothing.
The established bow rule remains active independently of crossbow preferences.
The user-facing refusal message now names both bows and crossbows. No new
setting or log destination is added.

## Validation

The new `tools/check_crossbow_merge_d83.py` compiles the literal production
quiver arbiter, reload-enable predicate and bow nock-commit branch.

- 210 checks pass in optimized and AddressSanitizer/UndefinedBehaviorSanitizer
  builds. The tests cover all three crossbow classes and the existing entry
  modes, stowed items, malformed equipment counts, disabled modes, belt/left
  hand preservation and the bow's exact release identity.
- The old bow-only arbiter fails 42 of those assertions.
- Claude's nock-commit branch fails the three acknowledgement/identity/time
  assertions in the same fixture.
- The existing actual D78 socket observer/writer suite passes 80 checks in
  debug, optimized and sanitizer builds. Its three restored-defect controls
  fail as intended, including an animated bolt-grip regression.

The D82 admission/clock/passenger suite is retained and rerun for this merge.
Its compact result receipt records the 606-check production bridge and the
older compatible ownership suites, alongside their causal negative controls.

Reproduce the new focused checks from the complete source root:

```sh
python tools/check_crossbow_merge_d83.py --out /tmp/kcd2vr-crossbow-d83 --baseline /path/to/reviewed/D82 --claude /path/to/KCD2VR_D83_SOURCE
python tools/check_crossbow_d82.py --out /tmp/kcd2vr-crossbow-d82
python tools/test_crossbow_socket_d78.py --out /tmp/kcd2vr-crossbow-socket
```

The package evidence includes literal Claude probes, exact source hashes and
the relevant numbered log excerpts. No test here executes the game, native
ammunition inventory, Windows detours or headset compositor. LeakSanitizer is
disabled for this host; AddressSanitizer and UndefinedBehaviorSanitizer remain
enabled in the reported sanitizer runs.
