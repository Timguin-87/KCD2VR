# Development notes

These notes keep the useful mistakes and decisions. The private recordings and
old build archives are kept separately; they do not need to ship with the source.

## D82: verify the connection the game actually uses

The November 2 capture identifies D81R1, so it supersedes the older D80 runtime
observations below. Crossbow reload was still falling back almost immediately.
The old fixtures used a one-slot root; the native assets use UpperBody 6+2 and
FullBody 0+3. Both production joins rejected those real profiles. The new test
starts without a completed reload source and follows the actual producer,
passenger consumer and completed-source producer through first admission.

The stereo incident exposed a separate gap: selecting DLSS in the launcher
changed user.cfg, but native TAA history separation did not create separate
vendor upscaler instances. D81R2 guarded that selection. D82 adds actual DLSS
and FSR context lifetimes and per-eye SDK routing. It preserves the accepted
camera and native AA code, and requires both vendor evaluations before accepting
a temporal pair. Host checks establish routing; only a Windows build and headset
run can establish the final image and performance.

Smithing also needed its real return path tested. A two-quiet-sample arming rule
could keep moving hands unarmed indefinitely after reheating. A fresh idle
down/up cycle can now arm the next lift while native strike rules remain in
charge. The combat recording proves one combo-marked damaging hit, not a finished
combo; its later expired final input motivated a separate, narrowly qualified
contact-timed transaction. See CHANGES_D82 for current results and limits.

## D81 correction to the history

For the earlier D81 repair, the November 1 D80 run was the latest headset evidence.
Earlier descriptions of a connected reload, mounted body, alchemy sound or map
optimization were implementation claims, not proof that the complete path worked.
The physical crossbow gate still depended on an unrelated speed preference;
write timing made the alchemy speed slider ineffective; and dialogue startup
could wait forever after an initially missing packet. D81 fixes those specific
connections and records which tests exercise them in CHANGES_D81 and VALIDATION.
Mounting now follows the user's clarified rule: native mounting finishes, then
normal VRIK starts after one second. It is not a separate mounted solver project.

## Hands first

I kept trying smoothing because the head was stable and the hands were not. More
filtering made them slow instead. That was the wrong thing to treat as progress.
The useful timing comparison finally improved empty hands, but drawing a weapon
could lose the improvement. It needs to be traced from the OpenXR sample through
publication, bones, attachment placement and presentation. Contact jitter is a
separate problem, even when both look similar in the headset.

The older fast build is a performance reference, not proof that its hands were
perfect. There were still disappearing arms and bad shelf contacts in that period.
It would be easy to remember only the good part and bring the same problems back.

## The arm did not need to become four metres long

Fixed upper-arm and forearm lengths were not enough. A contact point could end up
far from the controller, and shoulder accommodation followed it. The bones stayed
the same length while the whole result became ridiculous.

It now limits contact-driven displacement and drops an invalid contact instead
of keeping the wrist glued there. Station contact is deliberately simpler. A few
millimetres of penetration are less distracting than a hand refusing to follow me.
That still does not solve every shelf edge, finger or held-tool collision.

## A working console command was not a working grab

The console drag could move a corpse. It drove an entity's velocity toward a
point in front of the view for a few seconds. There was no persistent tracked-hand
grip, no reliable selected limb constraint and no shoulder carry.

The hand path added target selection and ownership, then managed to use the
contact-corrected wrist as its input in places. The body could stop that wrist
against itself. Moving the controller farther did not necessarily move the servo
target. That is a damn circular dependency. Acquisition and driving now use the
completed tracked hand before contact correction, while retaining the selected
body until release. Tests reproduce the old failure; an in-game result is still
needed before calling dragging reliable.

Living actors remain a different problem. Knocking them into ragdoll was not
natural yielding, and the dog flailing during petting was a regression. I am not
bringing that back as a shortcut.

## Alchemy almost worked, then lost the connection again

There was an early successful mortar tip into the cooking bowl. Later builds
could acquire herbs, plate and oil, but still waited around a second for native
attachment events. Then some integrations lost the contents-recipient path and
ordinary return behavior. Faster animation did not fix failed selection.

I want the hand to be the action. It still has to go through the correct native
Take, Return or contents-transfer operation, so quantities and recipes remain
consistent. A return-to-idle signal is not proof that anything entered the pot.
The remaining actions have to be completed individually. Zeroing the entire
activity would also remove grinding and other work events.

Names are separate from action selection. It can show the herb under my head
view without stealing the herb already targeted by my hand. The current patch
also restores release before delayed attachment, while cancelling that intention
if I grip again or the original object is no longer current.

## Smithing is worth preserving

The anvil part was nearly right in the last recorded run. The axe problem exposed
a different transfer: Henry leaves the workpiece in the forge rather than holding
it there forever. After a return from the anvil that could work; first entry was
wrong. This needs the hand-to-forge-to-hand transition, not another rewrite of the
working anvil code. The free right-hand bellows gesture stays available.

## Native pose flashing during draw

There are two different bugs here. A stowed bow leaning forward with Henry's
native torso animation was not the same as a newly drawn weapon showing its
native pose for a frame. Treating both as flicker kept sending the investigation
in the wrong direction.

The first attachment callback can happen before the normal equipment cache knows
about the item. Later hiding cannot protect an earlier visible frame. The new
path carries a short-lived attach candidate across that callback, checks the
actual current binding, and reveals only after matching tracked placement or a
real change of owner. A timeout is not evidence that placement succeeded.

It still needs real visual confirmation. Registration and write readback are
useful evidence, but neither tells us which pixels reached an eye.

## Reloads and paired combat cleanup

Releasing a crossbow tool during spanning cleared grasp, but there was no way to
set it again in that phase. The tool was still bound. Regripping now resumes that
same tool; it does not create a bolt or finish the reload without a measured stroke.

Temporary animation speed also needs proper cleanup. If a native read fails, it
does not mean the old action disappeared. It keeps the bounded restoration record
until current identity and speed can be checked, rather than losing responsibility
for a speed value it may have installed.

## Research and packaging

I asked Codex for static audits where the source and recorded behavior disagreed.
The useful output is the exact caller, ownership change and reproducible failure,
not a longer list of switches. lib2KCD and kcd2tools are reference points for game
data investigation. Crysis VR, PLANCK and FRIK are useful comparisons for VR
behavior; their engine interfaces are not a license to guess this game's ABI.

The source archive had grown mostly because an old combined companion still
contained the optional gore assets. Removing a few text logs would barely help.
The main build now has its own gameplay input, and the optional package stays
separate. Dependency notices and the files needed to reproduce a build remain.

Tests should say what they prove. A host fixture can catch a bad state transition;
it cannot certify a good-looking elbow, visible knife, successful shield bash or
comfortable roomscale. Those are still separate checks.

## Town maps

The current paper artwork lives in `companion/source/Objects/kcd2vr/staticmap40`,
not the old Books/Maps folder. It has 26 distinct sheets and covers all 24 names
in the native local-map catalogue. Keep those sets together. The selected sheet
is cached while the map stays open; changing areas may require closing and
reopening it. Swapping the texture alone would leave the model and grip joins stale.


## 2 November 2026: D83R1 and two independent D82 branches

The Claude D83 overlay and the reviewed D82 repair used the same D82 upscaler
filename for different interfaces. The overlay also kept its older D81R1 build
marker. A version label was therefore insufficient evidence of ancestry or
compatibility. D83R1 reconstructs the exact inputs, merges focused changes, and
uses all affected preimages to distinguish the supported complete trees.

The useful additions were the older capture's passive smith phase15 continuity,
held-LT native movement, crossbow shoulder arbitration and limited head turns
during native paired actions. The review retained D82 moving-hand rearming,
hand-owned reload clocks, final-contact timing and temporal presentation. It
removed the alternative shared-history fallback, a swallowed bow-release
assignment, a second binary crossbow cocking owner and an independent visual
workpiece offset. The finisher overlay gained source/context and restore guards
and a narrower anatomical mask that leaves torso equipment sockets native.

The combat history now labels recognizer onsets, gesture/contact IDs and exact
native outcomes separately. Matching a numerical fraction to the user's reported
miss rate did not establish the same denominator. Current implementation, host
verification, historical evidence and pending headset results are recorded
separately in CHANGES_D83R1 and VALIDATION_D83R1.
