# D81: repairs after the November 1 D80 run

This revision changes the supplied source. The observations below come from the
D80 recording and code; the new behavior has host tests, not a new headset result.

## Hands and collision

Optional precise finger recovery could spend the shared query budget before the
other hand's mandatory contact sweep. Failure of optional work could also discard
already measured current contact. Mandatory sweeps now run first, and optional
recovery failure does not invalidate a valid current result.

Station contact had a hard 20 mm threshold: a slightly larger measured correction
could switch the hand abruptly back to its uncorrected position. The presentation
correction is now bounded continuously. Its measured sweep origin remains separate
from that bounded display position so the next sweep does not start inside a wall.
Release and loss of coverage withdraw immediately; no stale surface is retained as
a hand anchor. This is still sparse contact, not full hand-mesh collision physics.

A carried object's initial offset was captured from a collision-corrected wrist,
then driven from the raw requested wrist. Acquisition and driving now use the
same completed requested wrist. Native pickup eligibility remains authoritative;
known eligible small items also regain their existing static-to-rigid preparation
during ordinary native Use, without F9. Unknown scenery is not made grabbable.

## Smithing and alchemy

The uploaded profile disabled LT anvil assistance. It is now enabled in the repair
baseline and one-time migration. Hold LT for native left-workpiece placement at
the anvil; release returns tracked control. Heating and transfers stay free.

Recorded native hammer impact often arrived 328–344ms after the VR release. The
acknowledged falling phase now advances at 4x with an 80 ms per-update authored-step
cap. Raise/reversal admission, heat, native hit events and world time are retained.
This repairs a timing mismatch; it does not manufacture a successful hit.

The alchemy slider had been writing CAF speed after the native action update and
restoring it before the next update. The action clock therefore remained at 1x.
Presentation speed now reaches the qualified native child action before Update.
The existing 8x setting applies to connected pickup, return, pot/bowl, resource and
book presentation. Gesture-owned grinding, bellows and work events keep their
own timing. Exact action identity and compare-before-restore protect transitions.

The delayed-attachment wait is no longer limited to the one flask/herb case.
Connected tools and non-herb ingredients can finish their actual native attachment
handoff without replaying the inventory action. Confirmed take/release feedback
uses the verified player audio proxy/trigger directly instead of requiring optional
AudioUtils, with authored native sound cues for herbs, bowls, flasks, hourglass and other
supported ingredients. An issued sound command is not proof of audible playback;
new generic particles and complete liquid simulation are not claimed.

## Physical crossbow reload

The physical reload setting was incorrectly coupled to the separate instant-reload
setting. The supplied profile had physical ON and instant OFF, so it could fall
back to a gesture starting a native animation. Physical string/tool/bolt ownership
now has its own gate. Fast reload transitions only control the native trailing
presentation.

The reload observer also cancelled a valid root reload when a secondary scope
reported metadata. It now follows the native root-scope test. Clock selection
permits an exact reload inside an ordinary bounded idle/reload blend. The initial
owned pause can wait for its first qualified pose within the original 80 ms deadline,
without renewing timestamps or creating gesture progress.

Light string pulling, real tool retrieval and lever motion, right-shoulder bolt
retrieval, visible native bolt binding and physical rail placement use the existing
mechanism. Releasing a bolt away from the rail leaves loading pending for regrip.
Native cock/load events and inventory remain the owners of the outcome. The bow
alignment and firing path are preserved.

## Dice and riding

Dice was included in the lens policy but excluded by the neutral-eye eligibility
mask. That left Henry's native inspection lean moving the view forward. Dice now
uses the same pre-entry head anchor for both the camera and a qualified torso/arm
presentation lease. Native seated pelvis/legs and dice outcomes remain native.
This does not add hand-controlled dice throwing.

Mounted handling now follows the requested ordinary animation handover: finish
native mounting, wait 1000 ms, then admit the existing normal VRIK path, including
unarmed riding. Gait or weapon changes do not restart that delay. Dismounting,
identity loss and protected native actions retire or suspend the handover through
the existing native action rules.

## Map and performance

Simple dark POI diamonds replace the icon-atlas marks on all 26 map sheets. Their
centres, the paper meshes and the player marker are preserved. Quest markers keep
D80's stable slots and unchanged-packet caching.

Paper lookup resolves the helper name once per activation, then checks the exact
entity ID in the current registry. Per-field VirtualQuery calls are removed from
paper reads and the quest reader's thrashing one-region cache. Bounded reads,
Windows exception boundaries, native identities and final ownership checks remain.
Off-sheet player markers use slot visibility instead of being moved 1000 m away,
which had expanded the helper's bounds. Pose expiry remains unchanged.

The D80 log records about 90 FPS with the map closed and 26–29 FPS open, plus recurring
long frames near the 500 ms quest interval. The repeated query cost is source-proven;
its exact contribution to each long frame and D81's actual FPS are unmeasured.

The supplied mod FPS cap is OFF. Its stored 90 value is inactive. The observed
90 FPS cadence is consistent with runtime/headset pacing but does not identify its
cause without runtime settings. Stereo scheduling and accepted hand timing were
not replaced as a speculative frame-rate fix.

## Dialogue, controls and files

The dialogue companion used an infinite wait if the first valid NPC packet was
not ready. It now retries only within that participant's native dialogue lifetime.
Saved NPC preferences install their qualified observers. Witnessed quiet after a
native subtitle clear can admit listening idles while choosing a reply; NPC speech
suppresses those gestures. Gaze still uses the exact admitted dialogue members.

Physical walking and headset dodge have independent menu controls. Enabling BODY
no longer silently enables walking. D80 recorded one actual dodge input request
with walking OFF, but native acceptance was unobserved. Halberd hit reliability
and the full dodge complaint are not presented as solved.

Station prompts default to 45%, retaining timed disappearance and palm reveal.
Throwing-knife acquisition and its menu controls are retired; the shared native
supply service remains because bow/crossbow and alchemy feedback use it.

The process logger is configured before hook startup. Ordinary messages, captures,
map timing, companion startup and finger-reference audit records use KCD2VR.log.
Finger references are kept in memory; no new BIN/JSON cache files are written.
The old stowed-bow and obsolete migration backups are no longer produced. One
last-good INI checkpoint is retained for settings recovery. Existing files on a
user's installation are not deleted by merely building this source.

The bundled profile preserves the uploaded settings and calibration except the
five requested repair choices and one migration marker. Existing profiles receive
those five choices once; subsequent user changes persist.
