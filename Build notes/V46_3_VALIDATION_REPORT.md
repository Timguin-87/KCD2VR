# V46.3 validation report

Local source/logic validation performed in the Linux analysis environment.

## Passed

- V46.2 menu/D3D12 structural suite: **27/27 PASS**.
- V46.3 safe-graphics suite: **PASS**.
- Verified profile only touches `e_ViewDistRatio`, `e_ViewDistRatioVegetation`, `e_MergedMeshesViewDistRatio`, `r_MotionBlur`, and `r_Sharpening`.
- Verified forbidden experimental culling/shadow/TAA/HLOD CVars are not in the V46.3 profile.
- Verified distance floors use `max(original, 120)` and therefore never reduce higher game settings.
- Verified snapshot, immediate rollback on partial setter failure, live disable restoration, and shutdown restoration paths exist.
- Verified F7 menu still uses the existing game D3D12 device/queue and the V46.1 10×5-second matrix remains intact.
- Verified OpenXR render-scale setting is persisted and clearly marked restart-required rather than falsely claiming live swapchain recreation.

## Build-environment limitation

CMake parsing reaches `find_package(DetourModKit CONFIG REQUIRED)` and stops because this Linux environment does not have the user's Windows DetourModKit SDK. A trustworthy Windows `.asi` was **not** built here. Use the normal fresh Windows/MSVC build procedure.
