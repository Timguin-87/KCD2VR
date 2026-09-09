# V61.32 retired-hypotheses update

This note is for future GitHub contributors so disproven runtime paths are not casually revived.

## Newly retired by the 2026-09-07 V61.30 runtime session

### Exact-source StereoL / StereoR producer capture (old F8)

Repeated F8 runs reported:
- binds = 0
- uniqueRT = 0
- sameEpochPairs = 0
- interpretation = NO-SAME-EPOCH-STEREO-PAIR-PROVEN

V61.32 removes this experiment from the F8 hotkey. The underlying dormant observer helper may remain in source history, but it is not armed by V61.32.

This does **not** prove KCD2 can never produce native stereo. It retires this exact framework-enable + exact-source-sized passive RT-pair hypothesis.

### V61.30 G-buffer 0x100 wrapper depth scan

The read-only probe reached tens of thousands of stage calls and roughly one million MRT observations in the supplied runtime session, but ended with zero candidate COM resources and zero depth hits.

V61.32 F4 never installs/enables this probe. Future depth work must trace a different owner/resource path rather than increasing the same wrapper scan.

### Final-skin CPoseData HAND pulse as visible VRIK owner

V61.30 produced hundreds of validated gate-2 HAND-APPLY/HAND-RESTORE operations on Henry's captured hand joints immediately around the selected final consumer, yet the headset showed no visible hand movement.

Do not make this pulse larger or repeat it as another ownership hypothesis. V61.32 retains it only behind F9's second READ+WRITE step for diagnostic comparison after a guaranteed read-only first step.

FULLBODY semantic mapping remains **unproven rather than disproven**: V61.30 correctly stayed at MAP-WAIT because the candidate semantic table did not validate against live Henry hand joint IDs, so it made no FULLBODY write.

## Existing immutable/retired boundaries still apply

Keep V61.12 gameplay-only culling and GOOD MONO immutable. Do not revive mode-2/Shadow, AFR, `+0x818/+0x828`, CSystem `+0x598`, blind renderer re-entry, old vcam/zero-camera, or OperatorQueue writes without genuinely new evidence that supersedes the runtime history.
