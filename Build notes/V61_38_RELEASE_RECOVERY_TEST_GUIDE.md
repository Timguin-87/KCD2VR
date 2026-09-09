# V61.38 Release Recovery — short runtime guide

1. Start in CORRECTED MONO. Confirm startup is 95/95 unless you intentionally changed Game FOV later.
2. Set FPS cap to match the headset refresh you actually use; the mod does not force 90.
3. Confirm head motion smoothness before enabling DEPTH PSEUDO.
4. Dialogue: use Force POV first. Transient zoom retain starts at 30%; lower removes more native dialogue/inventory zoom, higher keeps more.
5. Compare OLD MONO only as a selectable legacy mode; corrected mono remains the release candidate.
6. DEPTH PSEUDO: enable only when wanted. Depth work should disable immediately after selecting another presentation mode.
7. HUD scale: move the LIVE HUD slider and release it. A visible change is required to call it working. `REQUESTED` in the log without a visible change is not success.
8. Dialogue/subtitle text: same rule. A request log alone is not proof.

Useful sparse log markers:
- `V61.38-STARTUP BASELINE`
- `V61.38-FOV-DIAG`
- `V61.38-TRANSIENT-ZOOM enter/exit`
- `V61.38-HUD-SCALE WAIT/REQUESTED/FAILED`
- `V61.38-DIALOG-TEXT WAIT/REQUESTED/FAILED`
- depth mode transition/candidate lines
