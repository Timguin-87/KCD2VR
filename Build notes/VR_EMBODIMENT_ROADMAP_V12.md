---

# VR embodiment roadmap extension — V12

This section records the post-stereo embodiment target. It is deliberately staged:
**native stereo remains the gating milestone**, and these systems should be enabled
one at a time after stereo is stable.

## Design principle

Preserve KCD2 gameplay authority wherever possible:

**OpenXR physical input -> VR pose/constraint/intent -> native KCD2 action/state**

Do not replace KCD2 damage, stamina, inventory, crafting, AI, or combat semantics
with parallel VR-only systems unless no native bridge exists.

## Controller fingers

Initial Oculus/Quest Touch support should derive finger poses from controller
touch/button/grip state rather than assume full optical five-finger tracking.

Planned pose pipeline:

KCD2 animation -> body/arm IK overlay -> hand/finger bone overlay -> final skeleton

First implementation:
- thumb state from thumb/control touch
- index curl from trigger touch/value
- middle/ring/pinky curl from grip/squeeze
- object-specific grip poses
- fist detection from grip + trigger state

Later: investigate optional optical hand tracking separately.

## Physical crouch

Calibrate standing HMD height. Use relative HMD height plus hysteresis to request
KCD2's **native crouch action/state**, rather than merely lowering the camera.

Goal:
physical squat -> native crouch -> native stealth/movement/collision semantics.

Do not continuously toggle around the threshold; use separate enter/exit heights.

## Physical unarmed combat

A closed fist plus tracked controller velocity/orientation becomes punch intent.
Do not directly subtract NPC health.

Preferred path:
physical punch -> velocity/contact classifier -> native unarmed attack intent ->
KCD2 stamina/skill/armor/damage/reaction.

Forearm/fist guard poses may later feed native unarmed block intent.

## Shared physical constraint layer

Use a common controller-to-virtual-pose constraint/error model for:
- hand collision with world geometry
- NPC touching/pushing
- weapon collision
- heavy-weapon inertia
- stamina exhaustion
- grabbing/throwing
- smithing hammer contact
- mortar/pestle contact
- bow/string constraints

The real controller is never forcibly displaced. The virtual hand/tool can stop,
lag or deflect, and the controller-to-virtual error becomes a resistance/contact
signal and optional haptic source.

## Target lock and cursor

Desired VR behavior:
- no camera/HMD steering from target lock
- no aim magnetism
- no forced body rotation
- no permanent center cursor/crosshair

Keep internal combat-opponent knowledge if KCD2 requires it for AI, ripostes,
paired combat states or directional combat. Decouple that knowledge from camera
control rather than destroying the entire targeting subsystem.

Interaction feedback should migrate toward hand proximity, subtle highlighting
and haptics.

## Body-attached / gaze-revealed UI

Normal gameplay HUD should be nearly invisible.

Primary concept:
- left/non-dominant wrist: health, stamina, status
- weapon-hand gaze: weapon condition / handling information
- belt/pouch: quick inventory context
- shoulder/quiver gaze: arrow/ammo context
- quest/menu information only on intentional invocation

UI fades in only when the relevant body region is raised/oriented toward the HMD,
then fades away when gaze/pose leaves it. Avoid a permanent floating VR HUD.

## Stamina + RPG handling

Read native KCD2 stamina and keep native stamina drain authoritative.

Virtual weapon/arm response can combine:
- current/max stamina
- weapon Weight
- StrReq / AgiReq
- player Strength / Agility
- native attack/block/recovery state

Low stamina should reduce virtual acceleration/follow/recovery and increase
perceived weapon drag. Do not slow the real controller and do not directly
multiply damage; pass the resulting virtual blade/fist motion into native combat.

## Implementation order after stereo

1. Prove player skeleton and hand/head/pelvis transforms.
2. Prove native draw/holster attachment transitions.
3. Add body-relative virtual holster volumes.
4. Add Touch-derived hand/finger poses.
5. Add physical crouch -> native crouch action.
6. Remove camera lock/magnetism and permanent cursor.
7. Add gaze-revealed wrist/body UI.
8. Add fist classifier -> native unarmed attack.
9. Add shared hand/world collision constraint layer.
10. Integrate sword WeaponSolver targets and native directional melee.
11. Feed stamina/Strength/Agility/weight into virtual handling.
12. Add physical grabbing and native inventory transfer.
13. Add NPC touch/threat refinements.
14. Add physical bow/quiver/nocking/release.
15. Add physical alchemy actions.
16. Add physical smithing/tongs/hammer actions.

Each stage: **read-only observer -> gated experiment -> validated implementation**.
