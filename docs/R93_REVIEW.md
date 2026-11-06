# R93R2: complete source, reload handovers, crouch modes and pointing Quick Talk

This source starts from R92R4, commit
`da50832e9bca725c217e8393a6ddd71f7c76e3a5`. The export manifest records the
exact delivery commit and hashes every delivered source file. Extract this ZIP into
a new directory. It does not require an older ZIP or a sequence of copy-overs.
Keep the installed INI and calibration.

## Why the previous source did not compile

The supplied R92R1 build log reports three C2660 errors: the newer attachment
adapter calls `presentationOwns71` with five arguments, but the older alchemy
header accepts four. The extra argument identifies the physical hand for a
plate which occupies the native right-hand slot. Removing it would hide the
compile error by undoing the hand correction.

This tree includes the matching R92R2, R92R3 and R92R4 dependencies, including
the alchemy bridge, input, transfer and attachment changes. The previous pickup
handover corrections are retained. This packaging repair does not establish
that every pickup flash is gone in the headset.

## Crossbow source publication

The preserved latest capture has seven class-14 reload attempts. The controller
takes ownership and reports valid input, then reports `INPUT_LOST` only 30–63 ms
later, before the tool spawn. That is not proof of a headset tracking dropout.

The positive pose publisher cleared its interested flag and advanced its public
serial before copying the new pose. A concurrent native reload callback could
observe that temporary gap and permanently give the clock back to the native
animation. A normal update now leaves the preceding coherent packet readable
until the next packet has been published. Its original timestamp is retained;
the 80 ms freshness limit has not been extended.

Explicit withdrawal uses a cancellation epoch, so a publication already in
flight cannot revive a released or invalid action. Tracking, action, character,
weapon generation and exact native registration checks remain in force. The
publication is bounded and nonblocking; there is no added hand filter or lag.

## Right-hand bolt and tool placement

The reload already has a native-created bolt and a captured item-to-wrist grip.
The attachment callback nevertheless rejected its tracked target when the
native animation changed the slot's relative offset after the body commit.
Returning from that callback allowed a native placement for the same item.

For hand-driven reloads, the target now joins by the unchanged action, item,
entity and hand attachment identity. The exact current attachment descriptor
and completed body publication are checked immediately before and after the
write. A moving animation offset alone no longer hands the bolt/tool back to
the animation. The native/reference route keeps its original comparison.

The intended operation remains: crossbow in the left hand, string or tool
movement with the right hand, then the native bolt held in the right hand and
placed onto the crossbow. The same controller supports light, medium and
heavy crossbows (classes 10, 14 and 15). Native bolt creation, ammunition accounting,
reload events and loaded state are retained. No duplicate bolt or synthetic
ammunition is created to conceal a failed native handover.

These changes remove two concrete interruptions to that operation. They are
source/replay corrections, not a claim that all three crossbows have completed
a new headset test. A separate possible body/native-clock publication mismatch
remains under investigation; the available failure rows do not uniquely identify
it. No broader stale-pose retention was added for that hypothesis.

## Heavy crossbow lever after the chest grab

The heavy crossbow has a separate path: its native lever is attached to the
crossbow socket, then presented in the working hand before mounting. The
medium spanner and bolt use the player's hand attachment instead.

The heavy path discarded its captured grip when the native attachment's
relative offset changed, but allowed a new capture only at the exact spawn
edge. That made a later native offset update an unrecoverable placement
failure for the same tool. The correction keeps the original grip only while
the full tool, action and attachment identity still matches. Current native
mechanism measurements and the completed body publication remain required.
Once mounted, the lever follows the native weapon mechanism driven by the
tracked pull. A replacement tool or changed socket does not inherit the grip.

This is a separate source defect. The latest captured failures are class 14
medium-spanner attempts which abandon ownership before SpawnLever; they do
not establish that the heavy relative-offset defect occurred in that session.
The delivery report distinguishes their reproductions and retained checks.

## Bow arrow flicker while drawing

The nocked arrow belongs to the bow's authored draw chain, separately from
the quiver arrow held in the right hand. The latest preserved recording has
28 throttled draw-success rows. One reports a VR draw fraction of 0.977
while native charge is 0.557. A callback which yields to native can therefore
show a visibly different draw position even when controller tracking is valid.

The current draw callback separately reads physical and completed-body packets
and requires their sample and world transforms to match. A publication overlap
is a candidate for a skipped draw write, but these rows do not identify its
exact early-return condition. No bow aim, axis, charge, arrow lifetime or draw
geometry change is included for that unconfirmed cause. This delivery does not
claim the intermittent nocked-arrow flicker is solved.

## Crossbow support posture

While an actual left-primary crossbow has an owned right support hand, the
elbow solver favors a tucked position below the stock. Its existing torso/head
clearance and elbow continuity remain active. Wrist endpoints, grip transforms
and arm lengths are unchanged. The preference ends on support release and does
not apply to the working reload hand, ordinary weapons or the bow.

This addresses an elbow rolling up into the view while supporting the crossbow.
It does not move a tracked hand away from its controller to manufacture clearance.

## Impalement pin lifetime

The preserved capture also contains a corpse latch followed by releases after
47 ms and 78 ms. The source compared a newly published native pin against an
older tracking-sample clock. A valid pin could therefore appear to come from
the future and be dropped. The consumer now samples its clock after reading
the pin. The original tracking sample must still be at most 80 ms old, and the
pin must still be at most 150 ms old. Neither deadline is extended.

The replay reproduces that clock failure and exercises the actual physical
weapon and native coordinator through sustained following and deliberate
withdrawal. The capture did not record the pin timestamp at each release, so
the source defect is proven but its responsibility for every observed release
is not.

There is still a separate mode conflict between the rigid-sword constraint
route and the F9 corpse mode. This patch preserves the existing corpse-part
velocity fallback; it does not claim to provide a general living-NPC or rigid
sword-to-corpse constraint. Native hit and damage resolution are unchanged.

## Verification and limits

R93R2 completed the Windows Release/RelWithDebInfo link and proxy verification.
The final delivery manifest records its exact source and artifact hashes.
The earlier R93/R93R1 binaries do not contain the native crouch bridge.

The focused checks exercise the actual crossbow publication API, socket
placement, reload lifecycle and right support-elbow solver. Negative controls
restore the old defects and must fail. The impalement replay runs the actual
pin consumer, physical weapon step and native coordinator against fixture
native memory. Fixed-length and unchanged wrist endpoint checks are included.

The delivery report records the final test outputs, Windows build result,
source commit and SHA-256 hashes. Runtime fixtures are not headset acceptance.
The historical `run_v66108_checks.sh` entry point is blocked on this machine
because it requires `clang++`; the focused checks use MSVC instead. This is
reported separately from gameplay test failures.

Camera, stereo, DLSS/FSR, bow aiming, native arrow placement and the retained
pickup adapter were compared against the parent. No installed game files,
user settings or calibration were changed. The optional gore asset pack stays
separate from the main companion PAK. The timer remains paused.

## Building

Run `build_release.bat` from the extracted `KCD2VR_SOURCE` directory with
Visual Studio 2022 C++ tools, a Windows SDK, CMake and Python installed. The
patched DetourModKit dependency is supplied and verified by the build helper.
The pinned OpenXR SDK is downloaded on a first build unless its existing
source is supplied with `-OpenXrSdkSource`. A previous KCD2VR source tree is
not needed.

## Optional pointing Quick Talk

`VrGestureTalk` remains OFF by default. The release Immersion and Controls
menu and developer settings expose the same preference. An explicitly saved
ON choice is now respected rather than being reset by the old migration.

Look at a person or dog which has a native chat action, curl the other fingers
with Grip and leave the index finger off the trigger. The qualified pointing
gesture opens the native menu without the old 300 ms gesture dwell. Keep
pointing to use the native command choices; curling the index or ending the
point releases the gesture owner. The existing X route remains available.

The later target filter excluded dogs even though their retail script exposes
the same native Chat action. It now accepts that exact enabled dog action,
using the game's existing target selection and visibility checks. This is not
a new hand ray or a replacement dog-command system. Downward pointing can use
a lower hand position while retaining the forward/outward gesture and existing
holster, pickup, door and held-item ownership priorities.

This preference opens the chooser; raised hands or self-pats do not issue an
automatic battle cry. The existing rearm delay and command selection behavior
remain, so holding one point does not repeatedly reopen a completed command.

## HMD-height walking and controller crouch

Right joystick down already requests native crouch. The previous HMD-height
foundation removed native pelvis Z to suppress gait bob, which also removed
button crouch. The old Native choice restored the entire animated height path;
it was not a separate crouch correction, and the bow foundation still pinned it.

The Crouching choice now selects Physical or Native Button. Both retain the
HMD-height walking foundation. Physical leaves tracked head height direct.
Native Button also applies the game's accepted crouch state to the shared
head, hands and body foundation. The 0.5 m standing/crouched height difference
comes from the exact retail actor settings. It is composed once, after the
native/bow filter, and does not enter that filter's history or add hand lag.
Repeated callbacks for one controller sample keep the same stance, matching
the existing physical solver cache. The next fresh sample admits a stance edge.
Only that qualified shared height change is removed from fist/weapon motion
history; genuine hand movement, contact sweeps and collision positions remain.
This prevents a crouch from becoming a weapon jerk or cancelling a ready fist.

The state comes from the exact retail C_Player predicate used by the game's
player_in_crouch audio parameter. Its function bytes, retail fingerprint and
current player identities are checked. It is called only at the existing game
thread frame-end boundary; pose consumers read a bounded copied result. A
joystick press alone does not force the presentation into crouch if the game
refuses the action. The unchanged controller mapping sends that request.

This applies to ordinary gameplay, including a held bow. Stations, dice,
riding and native-owned actions keep their existing presentation owners.
A crouched world pickup carries its already-qualified pre-request foundation
through the existing acquisition reference; it does not gain a new pickup
permission or query native crouch inside the acquisition action.

The native addition starts OFF (Physical). A saved legacy native-height choice
is retained until you choose a new mode; the menu labels that legacy setting.
The shipped sample INI uses HMD-height walking. Existing installed settings and
calibration are not replaced. Use Immersion and Controls > Crouching, or the
same crouch choice in VRIK, to select Native Button.

Build products appear under `dist/prerelease`. Building does not install the
mod or replace the game's INI. See `BUILDING.md` for the normal build options.
