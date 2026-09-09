# KCD2VR V61.24 — Late Pose / COperatorQueue Write Proof

## Why this build exists
V61.23.1 proved F9 mode switching and live WeaponSolver/CPoseData capture, but produced no APPLY writes. The direct SetIKTargets write path is retired as a final-pose candidate.

Crysis VR writes arms from a skeleton post-process callback using SetPostProcessQuat. Modern CryEngine exposes the equivalent architecture through IAnimationPoseModifier/IAnimationOperatorQueue: queue joint overrides, then push the pose modifier into ISkeletonAnim.

The supplied WHGame.dll contains:
- `AnimationPoseModifier_OperatorQueue`
- `COperatorQueue`
- `IAnimationOperatorQueue`
- `IAnimationPoseModifier`

RTTI/vtable reconstruction identifies COperatorQueue's pose-modifier lifecycle. Its heavy Execute function is at **WHGame+0x6D7654**. Neighboring methods exhibit the expected double-buffer Prepare / Execute / Synchronize behavior.

## V61.24 test strategy
- WeaponSolver::SetIKTargets remains hooked, but **read-only**. It only publishes the player's proven CPoseData pointer, gate and candidate joint IDs.
- COperatorQueue::Execute is hooked at WHGame+0x6D7654.
- Original Execute runs first.
- Runtime validation requires `params+8` to have `Skeleton::CPoseData` RTTI and to equal the exact CPoseData last seen in WeaponSolver.
- Only after all validation does F9 mode permit a write.
- First proof modifies translation only (X, ±8 cm) through the already validated CPoseData getter/setter pair. Quaternion is untouched until component order is proven.
- OFF by default.

## F9
0 OFF -> 1 solver+0xC -> 2 solver+0xE -> 3 BOTH -> OFF.

Expected useful log markers:
- `V61.24-LATE-POSE MATCH ...`
- `V61.24-LATE-POSE APPLY ...`
- `V61.24-LATE-POSE OBS ...`

Interpretation:
- MATCH + APPLY + visible motion: late pose timing is proven; next step is controller orientation + two-bone arm IK.
- MATCH + APPLY but no visible motion: another modifier/skinning stage follows OperatorQueue; chase the generic pose-modifier dispatcher/final FK consumer.
- no MATCH: `SAnimationPoseModifierParams+8` is not the player CPoseData in this build/context; stay read-only and map the params rather than guessing.

No Windows/MSVC compile was performed in this environment.
