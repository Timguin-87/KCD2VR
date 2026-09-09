# V61.23 — Hand CPoseData Write Proof

## What WHGame.dll establishes

Static RTTI reconstruction for `Skeleton::CPoseData` yields its vtable and confirms the methods used by `WeaponSolver::SetIKTargets`:

- `WeaponSolver::SetIKTargets` = `WHGame+0xA7F9CC`
- dominant `gate414==2` path takes joint ids from `WeaponSolver+0x0C` and `+0x0E`
- CPoseData vtable `+0x70` = `WHGame+0x53F4D0`
- CPoseData vtable `+0x30` = `WHGame+0x53F510`
- CPoseData vtable `+0x50` = `WHGame+0x53D830`

Disassembly of `+0x70` and `+0x30` copies exactly `0x1C` bytes per joint (`jointId * 0x1C`) into two pose buffers. The 28-byte record matches CryEngine `QuatT` layout: quaternion (16 bytes) + translation Vec3 (12 bytes).

The `gate414==2` SetIKTargets path calls vtable `+0x70`. V61.23 uses the matching `+0x50` getter, copies the engine-authored transform, changes translation Z only, and calls the same `+0x70` setter. It never guesses a pose-array address and never patches the CPoseData object directly.

## Animation-mod evidence

The supplied combat mod includes `Libs/Tables/combat/combat_action_pose_modifier.xml` with partial-body pose-modifier records and Mannequin fragments. The supplied blacksmithing and alchemy mods contain dedicated Mannequin ADBs. WHGame.dll additionally contains `AnimationPoseModifier_JointPoseMorph`, `C_JointPoseMorphPoseModifier`, `AnimationPoseModifier_PoseAlignerChain`, `LimbIK_Definition`, `Animation_Driven_IK_Targets`, `BONEID_HAND_L/R`, and hand-IK debug CVars. This supports composing VR hands through the engine animation/IK pipeline rather than final-matrix replacement.

## Test behavior

Writes are OFF at startup.

Press **F9** to cycle:

1. `solver+0xC` joint pulse
2. `solver+0xE` joint pulse
3. both joints
4. OFF

The first press locks writes to the WeaponSolver instance that received that press so the experiment does not intentionally alter every solver in the scene.

Only `gate414==2` is modified in this build. Each selected joint receives a small sinusoidal **Z translation only**, max 3.5 cm. Quaternion is left untouched. The normal game SetIKTargets runs first every call; the proof write is applied afterward through CPoseData's own setter.

Look for:

- `V61.23-FRIK ... jointC=... jointE=...`
- `V61.23-HAND-WRITE F9 mode=...`
- `V61.23-HAND-WRITE APPLY ...`

If one F9 mode clearly moves one player hand, note whether it was `joint+0xC` or `joint+0xE`. That gives the hand mapping needed for the next controller-driven build.

## Safety / scope

- no write until F9
- no direct pose-array writes
- runtime vtable functions must exactly match the statically recovered WHGame RVAs or the write is skipped
- CPoseData RTTI must be present
- joint id must be 0..1023
- quaternion and translation must be finite/plausible
- write is translation-only and capped to 3.5 cm
- stereo, culling, FlashUI retry, HUD origin semantics remain from V61.21.1 baseline

## Next step after successful pulse

Use the confirmed hand joint id and CPoseData buffer semantics to convert OpenXR controller pose into character/model space. Then replace the proof pulse with one-hand controller translation, followed by controller rotation, and only after that solve elbow/shoulder through the engine's limb/pose-modifier stage.
