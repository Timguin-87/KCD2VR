# V62.1 headset test guide

## A. UI sliders

1. Press F7. V62.1 should open directly on `NATIVE KCD2 UI / VR SAFE AREA`.
2. Test one slider at a time, releasing the trigger/mouse after each change:
   - Subtitle size
   - Dialogue size / left-right inset
   - Interaction icon size / inset / vertical
3. Use `APPLY UI NOW` if needed.
4. Report whether any of the four target groups visibly moves/scales. If nothing changes, attach KCD2VR.log and note whether `V62.1 native UI apply=sent ...` appears.

## B. F6

F6 cycles three states:

1. AUTO
2. FORCE SCREEN
3. FORCE IMMERSIVE
4. back to AUTO

Test:

- startup/title -> screen, then normal gameplay -> immersive automatically;
- inventory via normal I / configured Touch inventory path;
- map via default M key fallback;
- pause via Escape / Touch pause;
- a cutscene/video;
- while inside a menu, cycle F6 through SCREEN -> IMMERSIVE -> AUTO and confirm AUTO recovers normally.

The sample `VrFlatScreenScale=1.70` makes the virtual screen slightly smaller than the prior 1.58 default. Right Grip + Right Stick still calibrates size/distance while screen mode is active.

## C. Live right-arm VRIK

1. Stand still in ordinary gameplay with empty hands if possible.
2. Keep the right controller close to its normal resting position.
3. Press F9 once. This enables LIVE RIGHT ARM and calibrates from the current native hand position.
4. Move the right controller only 10-20 cm left/right/up/down first.
5. Expected first milestone: Henry's right arm bends and the hand follows controller *translation*. Wrist/controller orientation is intentionally not implemented yet.
6. Press F9 again to turn the live arm OFF.

Stop the test and turn F9 OFF if:

- the wrong character moves;
- the arm explodes/twists violently;
- controller direction is clearly axis-swapped;
- performance/head tracking regresses.

For V62.1, keep F9 OFF during smithing, alchemy, pickpocketing and other authored hand minigames. Automatic action filtering is the next layer after ordinary controller tracking is proven.

Useful sparse log line while live:

`V62.1 VRIK live=... rig=... calibrated=... tracking=... allowed=... joints=... handR=... armLen=.../... solves=... rejects=...`
