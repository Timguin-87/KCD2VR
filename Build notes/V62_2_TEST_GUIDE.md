# V62.2 headset test

## 1. Native UI — test first

Open F7. The UI tuning page remains the same as V62.1.

Use deliberately obvious values first:

- Interaction size: 0.70
- Interaction inward: 30–40%
- Interaction vertical: about -100
- Dialogue size: about 0.55
- Dialogue inward: about 10–20%

Release each slider. The normal log should say:

`V62.2 native FlashUI apply=applied ...`

If it says `failed`, send the log; do not keep changing values.

If it says `applied` but a specific element does not move, report which element (Subtitles / DialogLeft+Right / Hints). That would isolate a MovieClip/state overwrite rather than the retail API bridge.

## 2. F6

No new F6 experiment was added. Confirm V62.1 behavior remains:

AUTO -> FORCE SCREEN -> FORCE IMMERSIVE -> AUTO

Inventory/map/menu/cutscene auto-screen behavior should be unchanged.

## 3. VRIK

Use normal gameplay, preferably empty-handed and standing still.

Press F9 once. Hold the controller near its current resting position for about one second, then move the right controller 10–20 cm slowly up/down, left/right, forward/back.

The log status should now show non-zero model-space arm lengths, roughly human-arm scale, and ideally increasing `solves=`.

Example shape:

`V62.2 VRIK ... armLen=0.xxx/0.xxx solves=... rejects=... failStage=0`

Interpretation if it still does not move:

- `failStage=1`: CPoseData absolute/relative vtable interpretation is wrong on this runtime.
- `failStage=2`: one semantic absolute/relative joint read failed.
- `failStage=3`: semantic arm hierarchy contains an unexpected relationship/helper chain.
- `failStage=4`: absolute buffer is not model-space positions as expected, or the correlated owner is wrong.
- `failStage=5`: controller target conversion/calibration invalid.
- `failStage=6`: geometry solve failed despite valid lengths.
- `failStage=7`: relative setter failed.
- `failStage=0` + `solves` rising + no visual movement: trace the exact relative->final consumer next; do not increase write amplitude or revive older write paths.

F9 OFF before smithing, alchemy, pickpocketing and authored hand minigames. Automatic action-state filtering is deliberately not guessed yet.
