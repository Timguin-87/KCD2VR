# V46.9 Test Guide

## 1. F6 flat UI first
1. Load into gameplay.
2. Open KCD2 Inventory, Quest Log, Map, or another flat menu.
3. Press **F6** once.
4. Expected: the whole KCD2 image becomes a smaller aspect-correct panel with black around it. Turn your head gently left/right/up/down; the panel should counter-shift and feel approximately anchored rather than glued to your face.
5. Press **F6** again to return to gameplay projection.

## 2. F7 MIS menu
Press **F7**. V46.9 automatically enables the same flat presentation while the parchment menu is open. Closing F7 restores the prior manual F6 state.

## 3. F8 combined audit (32 s)
- 0–5 s: mono 115° / framing 0.97 frame-identity reference
- 5–10 s: sequential view-identity observer (mono compositor fallback)
- 10–15 s: native pipeline-identity observer (mono compositor fallback)
- 15–20 s: native state-correlation observer (mono compositor fallback)
- 20–22 s: flat parchment boxing countdown
- 22–32 s: menu closes; throw normal unarmed punches with mouse/keyboard

Please note whether the 2-second parchment prompt is now clearly visible in HMD.

## Useful log markers
- `V46.9-FRAME-ID` — XR frame, PRESENT serial, PRESENT age, camera-apply age, source dimensions
- `V46.9-FLAT-UI` — flat UI state / anchor
- `V46.9-F8-BEGIN` / `V46.9-F8-END`
- `V46.9-BOXING-EVENT`

No unverified same-frame second render and no IK/joint write is armed in V46.9.
