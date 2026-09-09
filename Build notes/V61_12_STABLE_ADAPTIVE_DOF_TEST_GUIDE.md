# KCD2VR V61.12 — stability + adaptive pseudo DOF test

Source-only package. No Windows ASI was compiled in the Linux packaging environment.

## 1. Culling regression fix

V61.8's reversible pose sync remains, but V61.12 gates it to the exact gameplay CView camera before calling UpdateFrustumPlanes. Inventory/character/render-to-texture/auxiliary cameras are left untouched.

Expected log:
- `V61.12-CULL-SYNC gameplay CView only; matrix restored; FOV untouched`
- occasional `V61.12-CULL-SKIP auxiliary/non-gameplay camera left untouched`

Check first that world culling remains fixed and inventory/character-menu exposure is normal again.

## 2. Pseudo stereo

Manual presets remain:
- MONO
- PSEUDO .80
- PSEUDO .85
- PSEUDO .95 (default)

The validated fake comfort sweep remains available:
- .80 -> .85 -> .98, smooth global sweep

## 3. New ENGINE DOF button

V61.12 adds an `ENGINE DOF` button next to FAKE DOF.

This is deliberately read-only. The retail WHGame.dll contains the names `Dof_User_FocusDistance`, `Dof_FocusDistance`, and `FocusDistance`; V61.12 asks the CryConsole for those exact candidates and never writes any engine DOF value.

If one resolves, the menu shows focus distance and current pseudo scale. The curve is:
- <= 0.60 m: .80
- 0.60..1.50 m: .80 -> .85
- 1.50..4.00 m: .85 -> .98
- >= 4.00 m: .98

Output is smoothed with a 250 ms time constant.

Expected success log:
`V61.12-ENGINE-DOF focus=...m target=... smooth=... source=... readOnly=true`

If the exposed names are post-effect parameters rather than console CVars, the button will report unavailable and hold its prior pseudo scale. That is a safe negative result; use FAKE DOF instead and send the debug log.

## 4. Performance

- sys_MaxFPS target remains 90.
- Balanced profile remains 120 / 120 / 120.
- `e_MergedMeshesViewDistRatio` slider remains 30..250 with Apply / Original 30 / Balanced 120.

Test 30 first now that culling is fixed.

## 5. UI / letterbox

The V61.10 HUD safe-area PAKs and no-letterbox `fc_setRatioPreset` patch are RETIRED and intentionally not included in V61.12.

Reason from the runtime test:
- HUD.xml constraints repositioned/clipped the Scaleform stage instead of shrinking it; lower HUD icons disappeared.
- short-circuiting `fc_setRatioPreset` removed start-menu text while leaving unwanted bars.

Next UI target is semantic Scaleform layout scaling (`_xscale`, `_yscale`, `g_layoutWidth`, `g_layoutOffsetX`, action/reticle objects) and a narrow `ratioStrips` visibility change. Do not reinstall the retired test PAKs while testing V61.12.

## 6. FRIK recording

The read-only action recorder remains available. Record the desired action immediately before performing it, then send the debug log with the action name/order.
