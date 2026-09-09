# V62.9 VRIK implementation notes

## New runtime evidence used by this branch

The V62.6 diagnostic capture finally resolved Henry's semantic right arm as:

- UPPERARM_R 403, parent 395
- FOREARM_R 405, parent 403
- ELBOW_R 406, parent 403
- HAND_R 408, parent 405

The selector-1 positions produce stable UPPERARM->FOREARM and FOREARM->HAND distances around 0.266 m / 0.262 m across different poses. The companion/reference stream independently produces essentially the same segment lengths. This is the first runtime evidence that the exact correlated selector-1 records at the +0x5FE2F0 boundary contain usable positional arm geometry.

ELBOW_R 406 is not treated as the two-bone hinge in V62.9: it is a sibling of FOREARM_R under UPPERARM_R and its observed position matches the forearm position closely. The working chain is 403 -> 405 -> 408. Mode 3 moves the 406 helper together with 405 only as a conservative helper-preservation measure.

## DQ diagnostic correction

V62.6 computed final-output DQ candidate lengths from UPPERARM_R -> ELBOW_R -> HAND_R. That mixed the separate semantic helper into the chain. V62.9 captures final output for both FOREARM_R 405 and ELBOW_R 406 and computes DQ candidate lengths from UPPERARM_R -> FOREARM_R -> HAND_R.

No final-output DQ writes are enabled until one decoding convention remains geometrically stable under this corrected chain.

## Selector-1 PREBUILD setter

Retail WHGame disassembly confirms `WHGame+0x53F4D0` takes:

- RCX = CPoseData object
- EDX = joint index
- R8  = pointer to a 0x1C QuatT record

and copies the 28-byte record into selector-1 storage. V62.9 validates CPoseData vtable +0x70 against that exact function before any experimental write.

Modes 2/3 only write immediately before `WHGame+0x5FE2F0`, call the original hierarchy builder, then restore the exact original selector-1 records immediately afterward.

## Controller delta proof

V62.9 does not yet attempt a full production wrist/orientation solver. It first proves coordinate ownership with controller-relative movement:

`Cry local delta = { XR.x, -XR.z, XR.y }`

The baseline is the controller position relative to the HMD, captured after entering a write mode. This avoids adding gross head translation to the hand target during the proof. Delta magnitude is capped at 0.30 m.

- Mode 2 changes HAND_R translation only.
- Mode 3 solves positional shoulder->forearm->hand geometry while preserving native quaternions and the current bend side.

These are headset ownership tests, not release VRIK.

## Native animation-driven IK path

The supplied animation/mannequin data contains procedural IK handles `RgtArm01` and `LftArm01`, plus hand IK target templates. That path remains a high-priority production candidate. V62.9 only records the existing `WeaponSolver::SetIKTargets` correlation (`solver+0xC/+0xE`) and does not guess or overwrite the procedural IK ABI.

If the PREBUILD controller tests prove stable ownership, the next branch can compare this path against direct native `RgtArm01` target substitution before committing to a custom late arm solver.
