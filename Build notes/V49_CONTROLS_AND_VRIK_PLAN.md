# V49 controls / VRIK plan

## Existing useful data
KCD2 exposes BONEID_HAND_L/R, arm/forearm/elbow/clavicle IDs, LimbIK_Definition, Animation_Driven_IK_Targets, IKTargetProceduralClip, WeaponSolver::SetIKTargets, wh_am_DebugHandIK, GetBoneLocal, SetLookIK, and JointPoseMorph. OpenXR grip poses are already sampled for both controllers.

## First controls pass
Gameplay: Xbox-like semantic mapping. Left stick=move, right stick=turn, A=A, B=B, X=X, Y=Y, triggers=LT/RT, grips=LB/RB initially, stick clicks=L3/R3, left menu button=inventory (configurable), system/menu remains pause where available. Prefer injecting gamepad state rather than keyboard keys.

Menu mode: right stick/controller drives mouse pointer; A/right trigger click; B back/right-click where appropriate; stick scroll/menu navigation; left stick can provide d-pad-like navigation. Return to gamepad mode immediately when menu closes.

## Hands / body
First prove controller poses and game input. Then add experimental arm IK: native shoulder/clavicle roots + analytic two-bone solve to OpenXR grip targets, preserving native weapon attachment and WeaponSolver. Native leg locomotion stays active.
