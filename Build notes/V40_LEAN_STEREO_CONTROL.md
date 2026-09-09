# V40 Lean Stereo Control / Native Pivot

This build deliberately removes the retired V29-V36 runtime probe suite from the default path, adds live stereo-mode and eye-map controls, and makes eye swapping global at the OpenXR compositor mapping.

## Hotkeys
- Insert: cycle 0 Mono -> 1 AFR -> 2 Synchronized Sequential scaffold -> 3 Native Stereo scaffold -> 0
- Home: toggle global eye mapping

## Important honesty about modes 2/3
Modes 2 and 3 are control-plane scaffolds in this package. They are NOT relabelled AFR. Until a verified same-frame renderer re-entry/native dual-view hook is armed they deliberately fall back to the safe mono compositor path and emit `V40-NATIVE status`.

This is intentional: the known four-arg 0x653940 ABI and orphan CRenderView approaches remain forbidden, and V40 does not revive them merely to claim native stereo.

## Performance
`EnableLegacyStereoProbes=false` by default removes the V35 semantic gate and broad CView/pipeline observer detours that were firing at extreme rates in V39.
