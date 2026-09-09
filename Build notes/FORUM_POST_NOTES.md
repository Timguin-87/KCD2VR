# KCD2VR forum-post notes

## Short status

I am experimenting with a native OpenXR VR mod for Kingdom Come: Deliverance II, loaded as an ASI through the same `dinput8.dll` environment used by KCD2Tools/TPVCamera.

The project is not yet a playable VR mod. The current milestone is getting stable OpenXR/VDXR tracking integrated into KCD2's real camera before tackling stereo rendering.

## What is implemented

* OpenXR session creation with the game's real D3D12 device and DIRECT command queue.
* HMD pose polling at OpenXR predicted display time.
* Recentered 6DOF camera transform layered on top of KCD2's native eye pose.
* Camera modification at the verified TPVCamera frustum-builder hook, before cull planes are built.
* Quest/Oculus Touch OpenXR controller pose, trigger, grip and thumbstick actions.
* A tracking-only mode that does not create XR eye swapchains or issue custom D3D12 rendering commands.
* An optional purple diagnostic OpenXR swapchain path for testing compositor submission separately.

## What is not implemented yet

* Real left/right KCD2 stereo scene rendering.
* KCD2 weapon/hand transforms driven by VR controllers.
* Arm/hand IK.
* Roomscale locomotion/collision.

## Why Crysis VR is useful

Crysis VR is a useful architectural reference because it cleanly separates predicted OpenXR tracking, game-camera modification, controller actions, stereo submission, weapon transforms and roomscale. Its renderer implementation cannot simply be copied because KCD2 is a different CryEngine-derived game and uses D3D12, but the development order is very applicable.

## Useful technical references

* KCD2Tools / TPVCamera: https://github.com/tkhquang/KCD2Tools
* Crysis VR: https://github.com/fholger/crysis_vrmod

## Help wanted

The most useful help would be reverse-engineering of KCD2's main-view render dispatch / render targets, asymmetric projection setup, and player hand/weapon attachment transforms. Any CryEngine/KCD2 D3D12 experience would be especially useful.

## September 1 adapter/recenter correction

A live KCD2 test reached native D3D12 DIRECT-queue capture and the OpenXR D3D12 feature-level check successfully. The remaining `0x80004002 (E_NOINTERFACE)` adapter warning was traced to the mod attempting to query `IDXGIDevice` from `ID3D12Device`; this has now been replaced with the native `ID3D12Device::GetAdapterLuid()` comparison required for D3D12/OpenXR adapter validation.

The controller recenter action is also now edge-triggered (`changedSinceLastSync`) so holding X/A cannot continuously reset the tracking origin.

`DiagnosticRendering=false` remains the required test configuration. The current diagnostic GPU-submission path is not yet synchronized with KCD2's render thread, so purple-eye rendering should not be re-enabled until that producer/consumer handshake is restored.

## September 1 runtime milestone

A tracking-only build was tested with Virtual Desktop / VDXR. Head rotation and positional movement (including up/down and leaning) drove KCD2's desktop camera as 6DOF, while the previous purple-screen crash did not occur. The headset remained black because the safe build intentionally submitted zero visual composition layers.

The next branch restores render-thread synchronization for custom D3D12 work and captures the runtime's exact left/right eye poses plus asymmetric FOV values. This is a **native-stereo foundation**, not finished native stereo: the remaining engine-specific task is to identify KCD2's main-view render dispatch and make the engine produce two correct scene views without corrupting TAA/DLSS/frame-generation history.
