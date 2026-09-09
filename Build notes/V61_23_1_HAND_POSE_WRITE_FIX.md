# V61.23.1 Hand Pose Write Proof Fix

## Why V61.23 showed no movement

The test log proved the write mode never armed: every live `V61.23-FRIK` sample remained `writeMode=0`. The source had two independent F9 consumers. The old V54 `VrikBridge` polled F9 in the XR loop while the new pose proof used `GetAsyncKeyState(VK_F9) & 1` inside the SetIKTargets detour. The earlier poll could consume the low transition bit.

The same test session was also predominantly `gate414=1`, while V61.23 only allowed writes for gate 2. Therefore no CPoseData write could have happened even if the hotkey had armed.

## DLL-validated CPoseData pairs

WHGame static disassembly confirms matched pose buffers:

- gate 1 / buffer 0: getter vtable +0x10 -> WHGame+0x53D7C4; setter vtable +0x30 -> WHGame+0x53F510
- gate 2 / buffer 1: getter vtable +0x50 -> WHGame+0x53D830; setter vtable +0x70 -> WHGame+0x53F4D0

Both setters copy a 28-byte joint transform at `jointId * 0x1C`. No direct pose-buffer write is used by the mod.

## New F9 behavior

F9 is now edge-detected only in the reliable XR/update loop and cycles:

0. OFF
1. solver joint +0xC
2. solver joint +0xE
3. BOTH

Each press emits a distinct Windows `Beep()` and a `V61.23.1-HAND-WRITE F9 mode=...` line. The older V54 F9 toggle is retired for this proof.

When armed, the next live WeaponSolver instance is bound automatically. Gate 1 and gate 2 are both supported using their corresponding getter/setter pair. Translation Z only is pulsed by +/-3.5 cm; quaternion is untouched.

## What to look for

- `V61.23.1-HAND-WRITE F9 mode=1`
- `V61.23.1-HAND-WRITE bound solver=...`
- `V61.23.1-HAND-WRITE APPLY ... gate=1 ... api=buffer0 get+0x10/set+0x30`
- or gate=2 / buffer1 equivalents

If APPLY lines appear but no visible movement occurs, the next question is write lifetime/overwrite timing, not input arming.
