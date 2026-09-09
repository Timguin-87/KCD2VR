# Frame Nexus Probe

This build keeps the safe multi-frame `GetViewCamera` call-stack census and adds one runtime object inspection at the strongest main-frame path.

Static path in the exact WHGame.dll:

- `0x669C88..0x66A029` is a large frame/update orchestrator.
- It calls `CViewSystem::Update` at `0x669DD7`.
- Later it retrieves the persistent CSystem ViewCamera.
- It immediately passes that camera to the object stored at `WHGame+0x492D808` through virtual slot `+0x40`.

The probe logs:
- `FRAME-NEXUS global_rva=... object=... vtable_rva=... rtti='...'`
- virtual targets from slots `0x00..0x100`, including the observed `+0x40` method.

This is observer-only. It does not alter the renderer.
