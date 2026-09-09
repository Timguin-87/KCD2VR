# KCD2VR V62.5 — Touch Quick Consumables Test Guide

## Scope

V62.5 is a narrow input update on top of the V62.4 book-menu/F6/dialogue/VRIK-diagnostic baseline. Camera/culling, head-sync, F6 AUTO, UI tuning, graphics/view-mode pages, and zero-write VRIK diagnostics are intentionally preserved.

No Windows/MSVC compile or headset test is claimed by this source package.

## Corrected Touch mapping

### Left controller

- Left stick: movement.
- Left stick click: sprint / spur horse.
- Left Trigger: block.
- Left Grip: quick-action modifier.
- Left Grip + Left Stick Up: torch (`R`).
- Left Grip + Left Stick Down: helmet visor (`Z`).
- X: KCD2VR MIS menu.
- Y short: call horse/context (`X`).
- Y hold >= 0.5 s: hold companion/dog command (`X`).
- Left Menu short: inventory (`I`).
- Left Menu hold >= 0.5 s: pause (`Escape`).

The old Left Grip + Left Stick quick-weapon shortcut is removed.

### Right controller

- Right stick: camera / turn.
- Right stick Up: jump.
- Right stick click: crouch.
- Right Trigger: attack / release arrow.
- A: use / talk / pick up (`E`).
- B short: swap weapon slot 1/2.
- B hold >= 0.5 s: draw/holster current weapon by reselecting the active slot.
- Right Grip + Right Stick: existing theater/directional use remains owned by the existing runtime paths.
- Quest/Oculus system recenter is handled by the runtime reference-space recenter event; KCD2VR rebases camera/F6 anchors when that event arrives.

## New quick-consumable chord

KCD2 PC uses keys `5` and `6` for the first two consumable quick slots, and consumption is a hold action. V62.5 maps those directly:

- **Left Grip + Right Stick Left** -> hold keyboard `5` -> first/left quick-consumable slot.
- **Left Grip + Right Stick Right** -> hold keyboard `6` -> second/right quick-consumable slot.

Behavior:

1. Hold Left Grip.
2. Push Right Stick left or right.
3. KCD2VR presses and holds `5` or `6`.
4. Keep the chord held long enough for KCD2's native consumption progress to complete.
5. Returning the stick toward center, moving it vertical, switching sides, releasing Left Grip, opening the MIS menu, or disabling Touch bindings releases the held key.

Hysteresis is used (0.62 entry / 0.42 release) to avoid slot/key chatter near the stick threshold. While Left Grip is active, the right stick is reserved for this quick-action chord, so camera turn, opponent-switch pulse, jump, and right-stick directional mouse injection are suppressed.

## Regression checks

- Left Trigger still blocks.
- Left Grip alone does not block.
- Torch/visor require Left Grip, not Left Trigger.
- Left Grip + Left Stick Left/Right no longer selects weapons or consumables.
- Left Grip + Right Stick Left holds `5`; release returns `5` up.
- Left Grip + Right Stick Right holds `6`; release returns `6` up.
- Switching Left -> Right releases `5` before pressing `6` (and vice versa).
- X/Y/B/A/Menu behavior remains unchanged from the corrected mapping above.
- VRIK remains diagnostic-only; F9 performs no skeletal writes.

## Build verification

`build_release.bat` searches the freshly built ASI for:

`KCD2VR-V62.5-TOUCH-QUICK-CONSUMABLES`
