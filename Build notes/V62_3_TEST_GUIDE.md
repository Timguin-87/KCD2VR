# V62.3 headset test

## 1. UI live preview
Open F7. Drag one obvious control slowly, for example Interaction Y or Interaction inset.

Expected: the affected HUD element should change while the slider is held, not only after release.

If it changes only after release, or changes the wrong element, report exactly which control and which visible element moved.

## 2. VRIK diagnostics
Normal gameplay, empty-handed if practical. Press F9 once and move the right controller around slowly for 10-15 seconds. No arm movement is expected in V62.3; this build intentionally performs zero skeletal writes.

Then press F9 again to stop capture and upload:
- KCD2VR.log
- KCD2VR-diagnostics.log

The diagnostic line contains U/F/E/H semantic ids and parent ids, raw selector-0/selector-1/reference translations, raw final-output records, and four candidate decoded upper/lower arm lengths.

This one run should tell us whether the final 0x20 output can be decoded as model/hierarchy-space dual quaternions, or whether we should instead reconstruct model-space joints directly from the two local/reference QuatT streams and the recovered parent table.
