# V61.24.1 — Late Pose Companion CPoseData Proof

V61.24 proved `COperatorQueue::Execute @ WHGame+0x6D7654` is live and receives a CPoseData at `params+8`. Runtime evidence showed that pose is exactly `WeaponSolverPose + 0x68`. V61.24 rejected it because it required pointer equality, so no write was attempted.

V61.24.1 keeps the same conservative proof but accepts only:

- exact `poseCandidate == playerPose`, or
- observed companion relation `poseCandidate == playerPose + 0x68`.

The queue-side pose must still:

- have CPoseData RTTI,
- pass exact CPoseData getter/setter vtable validation,
- use the matched gate-1 or gate-2 buffer API,
- have sane QuatT data.

F9 cycles OFF -> joint +0xC -> joint +0xE -> BOTH -> OFF. The proof remains translation-only (local X, +/- 8 cm).

Expected markers:

```text
V61.24.1-LATE-POSE MATCH ... delta=104 ...
V61.24.1-LATE-POSE APPLY ...
```

If MATCH appears but APPLY does not, the remaining blocker is CPoseData API validation on the companion object. If APPLY appears but there is still no visible motion, the modification is being overwritten after OperatorQueue::Execute and we should trace the final skinning consumer instead of loosening this write further.

No Windows/MSVC build was performed or verified in this environment.
