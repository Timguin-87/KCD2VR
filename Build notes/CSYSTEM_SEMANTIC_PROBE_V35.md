# CSystem Semantic Probe V35

V34 resolved WHGame+0x492D8C0 to `CSystem` and vslot +0xF8 to WHGame+0x1A736D0. Static disassembly shows that function is only `movzx eax, byte ptr [rcx+0x598]; ret`.

Runtime V34 showed CSystem+0x598 stays false even while OpenXR has valid two-eye views, but the getter also has very high-volume non-stereo callers. Therefore V35 does **not** force the bit.

V35 remains read-only and adds:
- `STEREO-GATE-V35`: candidate getter result/caller.
- `CSYSTEM-SEMANTIC-V35`: byte snapshot of CSystem+0x580..+0x5C8 at selected calls.
- `CSYSTEM-VTABLE-V35`: neighboring CSystem virtual targets +0xD8..+0x128.
- `PIPELINE-V35`: run marker.

Goal: determine whether +0x598 belongs to a generic system-status cluster or a stereo/renderer-specific state before any causality write.
