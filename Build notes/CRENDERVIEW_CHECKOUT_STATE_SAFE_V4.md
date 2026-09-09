# CRenderView Checkout-State SAFE-V4

No new engine hooks are added. Renderer hot-path detours at 0x4E6924 and 0x4E7E64 remain disabled.

This build extends only the already-stable CRenderView manager checkout observer. For genuine mode-2 views it logs:
- `CRenderView+0x38` source/owner pointer, plus vtable/RTTI when readable
- `CRenderView+0x18`
- raw 16-byte state blocks at `+0x8D40` and `+0x8D50`
- a short checkout call stack for the first mode-2 checkouts

Key log markers: `CRV-CHECKOUT-STATE` and `CRV-CHECKOUT-STACK`.

No custom CRenderView is allocated or submitted. No stereo forcing.
