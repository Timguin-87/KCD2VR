# V49 implementation notes

The attached Cyberpunk/Crysis/REFramework research influenced the architecture, but V49 only enables changes that are safe against the currently identified KCD2 hooks.

Implemented now:
- runtime-derived XR refresh estimate,
- refresh/FPS A/B phases,
- pre-V44 raw aspect-fit control,
- corrected recenter translation basis,
- explicit timing/CVar logs.

Not falsely enabled:
- upstream asymmetric per-eye projection: needs a confirmed live KCD2 projection commit,
- downstream late camera write: needs a confirmed consumed camera matrix after C3DEngine/CRenderView handoff,
- additive menu-only OpenXR quad: needs a dedicated menu texture/swapchain rather than the baked game backbuffer.
