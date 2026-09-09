# V61.35 headset test guide

Keep the same V61.34 calibration/depth procedure. The only added test is F9.

## F9 — new final-output VRIK ownership proof

Start in ordinary first-person gameplay with Henry visible enough that a hand/weapon displacement would be obvious.

### First press — READ

Press F9 once. You should hear one beep.

Do not expect visible movement. Let it run for several seconds while moving/turning normally.

Expected log evidence:

- `V61.35-F9 READ ... ZERO writes`
- `V61.35-FINAL-OUTPUT MATCH ...`
- `V61.35-FINAL-OUTPUT READ ... valid=true ...`

The important fields are joint count, output pointer, live joints, and `valid=true` for the 0x20 transform slots.

If READ never reaches `MATCH`, or the slots stay `valid=false`, do not press F9 again for WRITE. Send the log instead.

### Second press — READ+WRITE

Only if READ produced valid final-output slots, press F9 a second time. You should hear the two-tone beep.

This does **not** write CPoseData. It modifies the post-animation final skinning output directly with a small oscillating translation-class perturbation for the two captured live joints.

Watch specifically for:

- hand movement;
- weapon movement/lag tied to the hand;
- any obvious oscillation in either captured joint;
- instability or mesh explosion.

Expected log if the new write actually executes:

`V61.35-FINAL-OUTPUT APPLY ... noCPoseWrite=true`

If you see visible arm/weapon movement, press F9 a third time to turn it OFF and record exactly which side/object moved.

If the log shows repeated APPLY but there is still no visible motion, this particular `+0x5FE2F0` output/index interpretation is not the surviving rendered owner and should not be made stronger.

## Other keys retained from V61.34

- F4: generic D3D12 depth-candidate observer + small depth-weighted modulation of the existing pseudo shift once a trusted candidate is found.
- F6: optional virtual screen only.
- F7: old calibration menu.
- F8: corrected projection toggle, default Center-Y trim +0.080.

V61.12 gameplay-only culling remains unchanged.


## F4 — generic depth + small pseudo-depth push

Use a pseudo mode before pressing F4 so any depth modulation is visually meaningful. Press F4 once and continue normal gameplay for roughly 10–20 seconds. The generic observer accumulates depth-stencil candidates and draw counts. Until a candidate reaches the live trust gate (screen + aspect match and at least 200 draws), the depth-push weight passed to the compositor is zero.

If a usable candidate is found, the log should show `V61.35-DEPTHPUSH bound ...`. The shader then uses the sampled depth only to scale the existing pseudo horizontal shift slightly; it is not full DIBR/reprojection. Press F4 again to stop the probe and print the ranked `V61.34-DEPTH-CAND` summary.

Please note any change in close-object/NPC depth impression, edge artifacts, reversed near/far feeling, flicker, or GPU instability. If near/far behavior is backwards, do not compensate elsewhere; report it with the log because the signed depth convention is still an explicit test variable.
