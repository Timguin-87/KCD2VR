# V61.34 headset test guide

## 1. Projection/framing calibration

Start in normal gameplay. F8 should already be ON.

Open **F7**. This is the older two-page calibration menu.

Recommended starting point:

- Game FOV: **106**
- Source FOV calibration: **106**
- VR zoom/framing: **1.00**
- F8 Center-Y trim: **+0.080**

The Center-Y line shows `trim` and `eff` (effective Center-Y). In the prior session +0.080 trim brought the automatic ~-0.087 correction to roughly -0.007, which was the visually good result.

Try Center-Y first around +0.06 .. +0.10. Then, without losing that value:

- vary **Game FOV** and press its visible APPLY button;
- vary **Source FOV calibration** independently;
- vary **VR zoom/framing**.

Please record the combination that gives the best world scale, least edge warp, and preferred apparent zoom.

Then compare the same calibration with:

- MONO
- PSEUDO .80
- PSEUDO .85
- PSEUDO .95

The projection correction is common to all of them.

## 2. Native HUD shrink

The bottom-left slider is the native HUD shrink test:

- 1.00 = 100%
- 1.50 = 50%
- 1.80 = 20%

Default is **1.50**. Test 1.40, 1.50, 1.60, 1.70 if needed. The slider request is attempted on release, but restart behavior is still the trustworthy test if live application is inconsistent.

Please note whether inventory, dialogue choices, compass, prompts and cursor remain visible/clickable after shrinking.

## 3. F4 generic depth observer

Do this in ordinary gameplay with a scene containing near geometry, NPCs and distant background.

1. Press **F4** once (high beep).
2. Look around / walk for **10–20 seconds**. Normal rendering should not visually change.
3. Press **F4** again (low beep).
4. Send `KCD2VR-debug.log`.

Useful log lines begin with:

- `V61.34-F4 GENERIC-DEPTH ON`
- `V61.34-F4 GENERIC-DEPTH OFF summary`
- `V61.34-DEPTH-CAND rank=`

The strongest result would be a high-draw candidate with `screenMatch=YES` and `aspectMatch=YES` at/near the active captured source dimensions.

## 4. F9

F9 is now only:

`OFF -> READ-ONLY -> OFF`

There should be no hand movement test and no CPoseData writes. It is retained only to correlate the late Henry/final-pose path while the actual final visible pose owner is researched.

## 5. What to report

Please report:

- best Center-Y trim;
- best Game FOV;
- best Source FOV calibration;
- best VR zoom/framing;
- which MONO/pseudo mode felt best;
- HUD shrink value that makes the UI comfortably visible;
- whether HUD needs restart to apply;
- F4 log.
