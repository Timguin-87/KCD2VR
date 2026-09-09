# KCD2VR V58.8 — F8 Native Stereo Ladder

V58.8 keeps the V58.7 gameplay baseline and extends the F8 audit with two black-box Crysis-style native render-twice probes.

## F8 order

The existing tracking phases run first, followed by the crop/pose isolation phases A-D, then:

- **CRYSIS_E_NATIVE_DUAL_EXEC** — calls the proven Normal `CStandardGraphicsPipeline::Execute` entry twice in one render epoch. The observed renderer camera bank is temporarily translated by `-IPD/2` and `+IPD/2` along its local right vector. Original camera bytes and selector are restored immediately afterward. XR output remains conservative; this phase proves/stresses render-twice re-entry.
- **CRYSIS_F_NATIVE_SELECTOR_TAGGED** — repeats the two executions and additionally sets the observed `CRenderView+0x8EE0` selector to `0` then `1`. Any PRESENT transitions occurring while an eye pass is active are tagged L/R, and the existing two-cache bridge attempts to preserve them as separate XR sources.

Neither phase is declared true stereo merely because the pipeline executes twice. True stereo requires runtime evidence that distinct L/R rendered resources survive and are submitted to matching XR eyes.

## What to report

For E and F, note whether the world becomes visibly stereo/deeper, one eye is wrong/black, the image alternates/flickers, shadows/particles duplicate, or there is no visible difference.

Please send `KCD2VR.log` and the debug log. Important markers:

- `V58.8-NATIVE-EYE-PASS`
- `V58.8-NATIVE-TAGGED-BRIDGE`
- `V56-F8-BEGIN` / `V56-F8-END`
- `V56-REENTRY-DIAG`
- `V46.5-AFR-BLIT`
- `V41-PRESENT-TAG`
- `V56-ANGULAR-WORST`
- `V56-CLAMP-STATS`

## Safety / rollback

The native phases are F8-only. They reuse existing guarded pipeline re-entry and temporarily modify only the already-observed renderer camera-bank/selector state; values are restored after each pair. The normal F8 completion and cancel paths restore mono bridge mode, fixed 8 ms tracking, crop preference, and default pose gating.

No VRIK/native skeleton writes were added.
