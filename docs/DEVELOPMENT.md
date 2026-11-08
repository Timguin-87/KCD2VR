# Development notes

the useful mistakes and decisions, short. the long version with every version tag is in
KCD2VR_HISTORY_AND_RESEARCH.md.

## verify the connection the game actually uses

more than once a fix was "done" and the game never went through it. example: crossbow reload kept falling back almost
immediately. the old test fixtures used a one-slot root, the real native assets use UpperBody 6+2 and FullBody 0+3,
and both production joins rejected those real profiles. so the tests were green and the game was not. a test has to
start from the real producer and follow it through the consumer to the first admission.

same with the stereo upscalers: selecting DLSS in the launcher changed user.cfg, but the native TAA history split did
not create separate vendor upscaler instances. the fix (D82) gives DLSS and FSR their own context per eye and needs
both eye evaluations before a temporal pair is accepted. host checks prove the routing, only a build and the headset
prove the picture.

and: an implementation claim is not proof. connected reload, mounted body, alchemy sound, map optimization were all
described as done before a run showed they were not. the crossbow gate depended on an unrelated speed preference, the
alchemy speed slider did nothing because of write timing, and dialogue startup could wait forever after one missing
packet.

## hands first

i kept trying smoothing because the head was stable and the hands were not. more filtering made them slow instead.
that was the wrong thing to treat as progress. the useful timing comparison improved empty hands, but drawing a weapon
could lose the improvement. it needs to be traced from the OpenXR sample through publication, bones, attachment
placement and presentation. contact jitter is a separate problem, even when both look the same in the headset.

the old fast build (C7E) is a performance reference, not proof that its hands were perfect. there were still
disappearing arms and bad shelf contacts in that period. easy to remember only the good part and bring the same
problems back.

## the arm did not need to become four metres long

fixed upper-arm and forearm lengths were not enough. a contact point could end up far from the controller and the
shoulder followed it. the bones stayed the same length while the whole result became ridiculous.

now contact-driven displacement is limited and an invalid contact is dropped instead of keeping the wrist glued
there. station contact is simpler on purpose. a few millimetres of penetration are less distracting than a hand that
refuses to follow me. that still doesn't solve every shelf edge, finger or held-tool collision.

mounting: native mounting finishes, then normal VRIK starts after one second. not a separate mounted solver.

## a working console command was not a working grab

the console drag could move a corpse. it drove an entity's velocity toward a point in front of the view for a few
seconds. no persistent tracked-hand grip, no reliable selected limb constraint, no shoulder carry.

the hand path added target selection and ownership, then used the contact-corrected wrist as its input in places.
the body could stop that wrist against itself, so moving the controller further did not move the servo target. a damn
circular dependency. acquisition and driving now use the completed tracked hand before contact correction, and keep
the selected body until release.

living actors are a different problem. knocking them into ragdoll is not natural yielding, and the dog flailing during
petting was a regression. not bringing that back as a shortcut.

## alchemy almost worked, then lost the connection again

there was an early working mortar tip into the cooking bowl. later builds could take herbs, plate and oil but still
waited around a second for native attachment events. then some builds lost the contents-recipient path and the
ordinary return. faster animation did not fix a failed selection.

i want the hand to be the action. it still has to go through the correct native Take, Return or contents-transfer
operation, so quantities and recipes stay right. a return-to-idle signal is not proof that anything went into the pot.
zeroing the whole activity would also remove grinding and the other work events.

names are separate from selection. it can show the herb under my head view without stealing the herb my hand already
targets. release before a delayed attachment is kept, and cancelled if i grip again or the object is not current anymore.

## smithing is worth keeping

the anvil part was nearly right. the axe problem showed a different transfer: Henry leaves the workpiece in the forge
instead of holding it there forever. after a return from the anvil that worked, the first entry was wrong. that needs
the hand-to-forge-to-hand transition, not another rewrite of the working anvil code. the free right-hand bellows
gesture stays. a two-quiet-sample arming rule could keep moving hands unarmed forever after reheating - a fresh idle
down/up cycle now arms the next lift, the native strike rules stay in charge.

## native pose flashing during draw

two different bugs. a stowed bow leaning forward with Henry's native torso animation is not the same as a newly drawn
weapon showing its native pose for a frame. treating both as "flicker" sent the investigation the wrong way.

the first attachment callback can come before the equipment cache knows the item. hiding it later can't protect an
earlier visible frame. the path now carries a short-lived attach candidate across that callback, checks the actual
current binding, and shows the weapon only after matching tracked placement or a real owner change. a timeout is not
proof that placement worked. a hide call or a readback is not proof of the first visible pixel either.

## reloads and paired combat cleanup

releasing a crossbow tool during spanning cleared the grasp, and there was no way to take it again in that phase while
the tool was still bound. regrip now resumes the same tool. it doesn't create a bolt or finish the reload without a
measured stroke.

temporary animation speed needs proper cleanup. a failed native read doesn't mean the old action is gone. the
restoration record is kept until the current identity and speed can be checked, so a speed value the mod installed is
never forgotten.

## research

static audits help where the source and the recorded behaviour disagree - the useful output is the exact caller, the
ownership change and a failure you can reproduce, not a longer list of switches. libKCD2 and KCD2Tools are my
reference for game data. Crysis VR, PLANCK and FRIK are good comparisons for VR behaviour, but their engine interfaces
are not a license to guess this game's ABI.

tests should say what they prove. a host fixture can catch a bad state transition, it can't certify a good looking
elbow, a visible knife, a shield bash or comfortable roomscale.

## packaging

the source archive had grown mostly because an old combined companion still had the optional gore assets inside.
removing a few text logs would barely help. the main build has its own gameplay input now and the gore package stays
separate.

## town maps

the paper artwork lives in `companion/source/Objects/kcd2vr/staticmap40`, not the old Books/Maps folder. 26 sheets,
covering all 24 names in the native local-map catalogue. keep them together. the selected sheet is cached while the map
is open, changing areas may need close and reopen. swapping only the texture would leave the model and grip joins stale.
