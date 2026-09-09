# V46.3.1 WHGame.dll / Overlay Static Crash Audit

Basis: uploaded `WHGame(1).dll`, current 64-bit build lineage used by V45/V46.

## What can be validated here

This Linux environment cannot execute KCD2, VDXR, or the Windows D3D12/OpenXR runtime. It therefore cannot prove that a Windows runtime crash is impossible. It can statically validate the engine-facing ABIs/RVAs and audit the D3D12 state/recursion design.

## WHGame hook/RVA checks

- `WHGame+0x7F0AE0` (ViewCamera publish): executable thunk. Entry is `add rcx, 0x288; jmp WHGame+0x426E80`; RDX is passed through, consistent with the existing `(system, camera)` detour ABI.
- `WHGame+0x57278C` (`CStandardGraphicsPipeline::Execute`): executable normal prologue. It reads `self+0x1978`, then passes `CRenderView+0x9B30` and count `+0xA330` to `WHGame+0x653940`. V46.3.1 does not alter the five-argument ABI rule for `0x653940`.
- `WHGame+0x43EB60` (`GetOrCreateRenderView` candidate): executable normal prologue; `movsxd rax, edx` consumes the second argument as an integer `EViewType`, consistent with `(renderer, int view_type)`.
- `WHGame+0x2525414` (recursive-pass builder): executable normal prologue; preserves/uses EDX, R8 and R9 in addition to RCX, consistent with the existing four-argument observer detour.
- `WHGame+0x492D8A8` lies in `.data` and is only read under SEH as the IConsole global pointer.

## CVar ABI check

At `WHGame+0x245F33D`, the uploaded DLL itself calls console vslot `+0xB8` to fetch a CVar and then ICVar vslot `+0x38` to set an integer value. V46.3 uses that same interface family; every pointer dereference/call is isolated behind SEH leaf helpers. The Balanced profile is reversible and only touches ordinary shipped CVars.

## Menu/D3D12 safety audit

- Menu creates **no second D3D12 device**.
- Menu command list is tagged as internal and excluded from the game's ResourceBarrier/PRESENT capture hook.
- Menu submits through `ExecuteCommandListsBypass`, so its own queue submission cannot recursively re-enter the detour callback.
- Menu only injects after the exact game command-list submission that contained the observed PRESENT transition.
- Captured backbuffer is transitioned `PRESENT -> RENDER_TARGET -> PRESENT` in one menu command list before the XR bridge uses it.
- A fence is waited before resetting the menu allocator/list.
- Menu is drawn before the existing XR blit; the XR blit then independently transitions the same source `PRESENT -> PIXEL_SHADER_RESOURCE -> PRESENT`.
- Menu creation/render failures return false and leave the existing VR bridge active; HWND/menu initialization failure falls back to hotkeys.
- V46.3.1 parchment styling changes only palette/font/border drawing in `vr_menu.cpp`; no hook, resource-state, XR, CVar, or stereo logic was changed from V46.3.

## Result

**STATIC AUDIT: PASS for the uploaded WHGame.dll build.**

This substantially reduces avoidable crash risk, but only a Windows KCD2/VDXR launch can validate driver/runtime behavior. A crash-free runtime is not guaranteed by static analysis.
