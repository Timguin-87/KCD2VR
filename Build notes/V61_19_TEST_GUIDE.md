# KCD2VR V61.19 — HUD + focused stereo chase

Source-only test branch. No Windows ASI was compiled in this environment.

## What changed from V61.18

- Keeps HUD/FlashUI discovery as the highest priority.
- Runs the HUD engine-global RTTI scan in every normal VR session.
- Also installs a *narrow, read-only* stereo observer in the same run:
  - CSystem stereo semantic gate (`CSystem` vslot `+0xF8`, observed field `+0x598`)
  - `CStandardGraphicsPipeline` dispatcher at current `WHGame+0x57278C`
- The pipeline observer records the live stereo CVar backing values, VolCloud LeftEye anchor metadata, pipeline object census, and a small initial pipeline-state diff.
- It does **not** enable stereo, flip the CSystem field, write a camera, write a projection, or alter render targets.
- Does not enable the older high-volume CView/entity/GBuffer observer suite.
- FOV defaults remain 95/95.
- Rotation-order control remains removed.
- Head-translation scale control remains removed/fixed at 1.00.

## Short test

A 20–40 second normal gameplay run is enough. No FRIK recording is required.

Send the debug log. Useful lines begin with:

- `ENGINE-GLOBAL-SWEEP-V36`
- `V61.17-HUD-FLASH-DISCOVERY`
- `V61.19-STEREO`
- `STEREO-GATE-V35`
- `PIPELINE-V29`
- `STEREO-RENDERER-V33 direct`
- `VOLCLOUD-STEREO-V36`
- `PIPELINE-STATE-V36`
- `PIPELINE-OBJECT-SWEEP-V36`

## Goal

HUD: identify the live `CFlashUI`/UI singleton so the next patch can expose real HUD scale/X/Y and independent cursor X/Y/reset controls in the VR menu.

Stereo: identify whether the current CSystem gate and standard graphics-pipeline state expose an eye/pass discriminator that can be commandeered UEVR-style. This branch is observation-only so it should not disturb the working pseudo-stereo/culling path.
