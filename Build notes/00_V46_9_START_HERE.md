# MIS V46.9 — Frame Identity + Flat UI + Boxing

This build keeps the provisional Mono Performance baseline at 115° / framing 0.97 and focuses on three confirmed problems from V46.8: head-motion judder despite good FPS, flat menus being projected nose-close, and the 2-second boxing prompt being easy to miss in HMD.

## Hotkeys
- **F6** — toggle the manual flat-UI prototype for KCD2 Inventory / Quest / Map / other 2D screens. It shrinks the game image into a black-surround virtual panel and anchors its orientation when enabled.
- **F7** — MIS parchment menu. V46.9 automatically puts the MIS menu through flat-UI presentation while it is open.
- **F8** — 32-second combined audit:
  1. Mono frame identity — 5 s
  2. Sequential view identity observer — 5 s
  3. Native pipeline identity observer — 5 s
  4. Native state correlation observer — 5 s
  5. Flat boxing countdown — 2 s
  6. Targeted boxing capture — 10 s

Modes 2/3 remain safe mono compositor fallbacks. No unverified second-render call is armed in this build.

## New timing evidence
`V46.9-FRAME-ID` ties the active XR frame to the exact PRESENT resource consumed by the bridge and logs `presentAgeMs` plus `cameraApplyAgeMs`. This is intended to distinguish ordinary FPS problems from pose/render submission mismatch.

## Flat UI scope
The F6 implementation is a prototype using the existing projection swapchains, not a final OpenXR quad layer. It preserves source aspect, adds a black surround, and counter-shifts against head yaw/pitch from an anchor orientation so the panel behaves approximately world-fixed. The final design can move to an OpenXR quad layer once the ergonomics are confirmed.

## Boxing
The countdown forces flat-UI mode for visibility, closes automatically, then captures mouse/keyboard boxing for 10 seconds. It remains passive: no IK/joint writes.
