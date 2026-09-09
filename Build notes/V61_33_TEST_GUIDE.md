# KCD2VR V61.33 — combined headset test guide

This is a **source-only test package**. Build it on Windows/MSVC using the corrected CMake list (`dxguid` is present). Do not treat source/static validation as runtime proof.

## 1. Baseline first

Start in ordinary immersive GOOD MONO. Confirm normal gameplay/head tracking and V61.12 culling behavior before pressing test hotkeys.

Do **not** use F6 for inventory/dialogue during this test. Inventory/dialogue are meant to stay immersive.

## 2. Inventory / dialogue forced zoom

Default:

```ini
VrTransientZoomRetain=0.40
```

Open inventory and enter dialogue normally in immersive mono. Also draw/aim the bow.

Desired result:

- inventory/dialogue stay immersive;
- remaining zoom-in is noticeably weaker than the previous build;
- bow aim sensitivity feels unchanged;
- bow FOV zoom is largely suppressed;
- no repeated FOV pumping after returning to gameplay.

Useful log markers:

```text
V61.33-TRANSIENT-ZOOM enter ... retain=0.40
V61.33-TRANSIENT-ZOOM suppress ...
V61.33-TRANSIENT-ZOOM exit restoreGame=...
```

The Dialogue tab exposes `Forced zoom retained` from 0–100%; 40% is the starting point. Lower = less native zoom retained.

## 3. Native HUD scale — highest-value UI test

Wait until normal HUD is visible. Open the VR settings book, choose **UI**, then move the `Native HUD scale` slider and **release it**.

Suggested sequence:

- 0.90
- 0.85
- 0.80
- 0.75
- 1.00 restore

Check compass, action prompts, subtitles/dialogue, inventory text, and cursor/select alignment.

Useful markers:

```text
V61.33-HUD-SCALE WAIT ...
V61.33-HUD-SCALE REQUESTED scale=... semantic=UIAction.SetScale ... executeSlotCandidate=+0xD0
V61.33-HUD-SCALE FAILED ...
```

Interpretation:

- **HUD visibly shrinks/grows and interactions remain aligned:** major proof; root Scaleform scaling works and can become the real beta UI slider.
- **REQUESTED but nothing changes:** likely retail `#` Lua execution restriction or the inferred IConsole ExecuteString slot/command path is not valid. Do not revive HUD.xml clipping. Next step is direct static tracing of the MovieClip transform ABI.
- **Crash on slider release:** immediately stop testing this path and send the log. Default 1.00 does not execute the candidate at startup; the call is user-triggered only.

## 4. F6 — optional virtual screen

Press F6 once: LOCAL/world-locked virtual screen ON.

Press F6 again: immersive projection restored.

Right Grip + Right Stick still adjusts F6 distance/size. This is completely separate from Native HUD scale.

Inventory/dialogue do not automatically engage F6 anymore.

## 5. F8 — automatic XR projection

Press F8 once. The build derives the enclosing source coverage and vertical optical center from live OpenXR L/R tangents and the actual captured source aspect. There is no fixed 106-degree target.

Start with trims at zero and Presentation Zoom at 1.00. If needed, make only small personal corrections in the FOV / Projection tab:

- Auto FOV trim: ±10 degrees
- optical-center trim: ±0.08
- presentation zoom: 0.95–1.05

Compare both Render = Mono and Render = Fake Stereo while F8 is active. If this removes/reduces the warp, it becomes the common projection base for GOOD MONO and pseudo stereo.

Press F8 again to restore prior game/source/Center-Y state.

## 6. F4 — focus pseudo

Test F4 after the projection/UI tests. Move gaze between near and far geometry for several seconds.

Useful markers:

```text
V61.32-F4 SOURCE-VALID focus=... target=... smooth=... depthProbe=RETIRED
V61.32-F4 SOURCE-MISSING ...
```

The old G-buffer wrapper scan is not re-run.

## 7. F9 — read before write

Press sequence:

1. F9 -> READ: correlation/observation only, **no pose writes**.
2. F9 -> READ+WRITE: existing validated temporary HAND pulse may execute.
3. F9 -> OFF.

The previous runtime result already showed final-skin hand writes with no visible hand movement. This state arrangement is for safer logging/ownership comparison, not to re-litigate OperatorQueue or claim VRIK.

## What to send back

Please send the normal and debug logs plus short notes for:

- native HUD slider: which values changed the HUD, whether inventory/dialogue/cursor remained aligned;
- inventory/dialogue zoom at 40%;
- bow zoom suppression;
- F8 auto projection: Mono and Fake Stereo warp/scale/edge behavior;
- F4 near/far pseudo impression;
- F9 READ vs READ+WRITE, especially any sword/hand side effect.
