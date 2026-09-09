# V61.1 PREP

Changes applied to the V61.0 CULLFIX source baseline:

- Exact-CView SEH crash guard retained.
- TPVCamera/KCD2Tools-style FOV+cull tuple is now left persistent after UpdateFrustumPlanes instead of immediately restored.
- Default render+cull tangent scale changed to 1.60x for an obvious diagnostic.
- Mono cyclopean default changed to true common-eye mode (0).
- cl_hfov menu control retired; prior test proved it cross-couples cl_fov and warps.
- Menu can load `MIS_MENU_BOOK.bmp` beside `KCD2VR.asi` as a real illuminated-manuscript/book background; procedural parchment remains fallback.

## Not yet claimed implemented
Depth-adaptive stereo still requires resolving the Normal GBuffer depth wrapper to the actual D3D12 depth resource/SRV. The project already has the GBuffer MRT hook and DOF-depth evidence, but wiring an opaque engine depth object blindly into the compositor would be unsafe. Do not label pseudo modes as depth stereo until this resource identity is validated at runtime.
