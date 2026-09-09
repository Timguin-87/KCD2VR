# KCD2VR / Medieval Immersion Simulator V58.2

## Scope
V58.2 is a deliberately narrow test build on top of the V58.1 build-fix baseline.
It does **not** change tracking, projection, sharpening, XR swapchain recreation,
dialogue camera behavior, stereo experiments, or native skeleton memory.

### Changes
1. **Corrected MIS menu navigation**: the **LEFT Touch stick** is now the book/D-pad navigation stick, matching the authoritative binding specification.
   - Right-hand controller ray remains the pointer when the flat-quad fallback is active.
   - Right Trigger still clicks/drags menu controls.
   - Right A selects and Right B closes.
2. **Raw Touch-state telemetry** is written roughly twice per second while Touch bindings are active.
   - Logs both thumbsticks, stick clicks, triggers, relevant face/menu buttons, and grips.
   - **Right grip is only logged as `[RESERVED]`; it remains completely unmapped.**

## First test
1. Boot and confirm the log contains `V58.2 ready`.
2. Open MIS with F7 or Left X.
3. Move the **left stick**: selection should move/adjust in the book.
4. The **right stick must not navigate the MIS book**; it remains the gameplay locomotion stick when the book is closed.
5. Use Right A / Right Trigger to select. Right B should close the book.
6. Close the book and test normal right-stick movement plus the rest of the provisional Touch bindings.
7. Send `KCD2VR.log` and the debug log. Searchable telemetry prefix: `V58.2-TOUCH-STATE`.

## Safety / architecture status
- Production XR bridge is still mono copied to both eyes; this is not true stereo.
- VRIK remains read-only/diagnostic. `nativeWrites=0` is still the requirement.
- Right grip remains reserved for future holster/object interaction.
