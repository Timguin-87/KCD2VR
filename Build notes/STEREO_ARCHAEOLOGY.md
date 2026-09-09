# Stereo archaeology probe

This build performs a one-shot, read-only scan of loaded module image memory after the
game's D3D12 DIRECT queue is captured.

It searches for historical CryEngine stereo/VR identifiers including:

- `sys_vr_support`
- `r_stereomode`
- `r_stereodevice`
- `r_stereooutput`
- `r_stereoScaleCoefficient`
- `hmd_driver`
- `IHmdManager` / `IHmdDevice`
- `SAsymmetricCameraSetupInfo`
- `CD3DStereoRenderer`
- `StereoRenderer`
- `Dual rendering`

Hits are written to `KCD2VR-debug.log` as:

`STEREO-ARCH hit module='...' rva=0x... string='...'`

A missing string does NOT prove the code is absent: retail builds can strip CVar names,
RTTI, logging text, and symbols. A hit is useful evidence and gives us an RVA/module to
investigate next.

This probe does not enable any discovered CryEngine stereo functionality.


## KCD2-specific multi-view/render anchors

The probe also searches for concrete console strings from the KCD2 console-command dump:

- `r_3MonHack` and its HUD/left/right offset settings
- `r_ShowRenderTarget`, `r_ShowDynTextures`, `r_ShowTexture`
- D3D12 resource tracking / PIX runtime toggles
- Warhorse frame-duplication and reprojection settings
- render-job, recursion, portal, and frustum-related settings

The highest-value hit is currently `r_3MonHack`: a surviving multi-view renderer path may lead
to the code that builds shifted projections or dispatches multiple scene views. This build only
locates strings/module RVAs; it deliberately does not patch or enable those engine features.
