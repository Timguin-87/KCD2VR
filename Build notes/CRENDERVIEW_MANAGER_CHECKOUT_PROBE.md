# CRenderView manager checkout observer

This build is observer-only. It does **not** allocate a second CRenderView, force stereo CVars, execute vr.cfg, or change render submission.

New hook: `WHGame+0x16F78F4`.

Static analysis shows this function:

1. Reads a provider object from manager `+0x90`.
2. Calls provider virtual slot `+0x10` to obtain a managed view object.
3. Appends the returned pointer to the manager vector at `+0x08/+0x10/+0x18`.
4. Registers pointer -> vector index through manager state at `+0x38`.
5. Clears manager field `+0x50`.
6. Returns the managed object.

There are two direct call sites in this exact WHGame.dll:

- `WHGame+0x4E6F4B` -- immediately adjacent to the renderer/view orchestration path already under investigation. The caller then writes a source pointer to returned object `+0x38`, calls vslot `+0x10` with source `+0x2C`, and copies source fields at `+0x8D40/+0x8D50` into the checked-out object.
- `WHGame+0x43EBBE` -- a second managed-view consumer.

New log records begin with `CRV-CHECKOUT-OBS`. Correlate each checkout with existing `CRV-FACTORY-OBS` and `CRV-CTOR-OBS` records. The useful fields are caller RVA, manager, provider RTTI/vtable, returned RTTI/vtable, CRenderView `+0x14` mode, vector count, last pointer, and `registered=true`.

The objective is to prove which managed CRenderView family feeds the main world renderer and identify a lifecycle-safe route for a second eye without directly constructing an orphan CRenderView.
