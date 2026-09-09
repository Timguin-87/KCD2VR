# V46.2 Control Menu Architecture

## Why this implementation

A separate ImGui/D3D12 overlay that creates its own D3D12 device would repeat a crash pattern already retired in this project. V46.2 therefore uses a very small custom immediate-mode panel with no third-party UI dependency and no additional graphics device.

The CPU side draws the panel with GDI into a 650×820 BGRA bitmap. The existing game D3D12 device owns a small upload buffer, one UI texture, descriptor heaps, a root signature, and a blend PSO. The menu submits its own command list through `DeviceHook::ExecuteCommandListsBypass` so the queue hook cannot recurse.

`DeviceHook` now tracks which game graphics command list recorded the latest PRESENT transition. The menu only injects when that particular command list has actually been submitted. Its own command list is registered as internal and excluded from PRESENT/resource probes.

## HMD visibility

`Mod::OnRenderTick` calls:

```
VrMenu::OnRenderThreadTick(queue)
XrManager::OnRenderThreadTick(queue)
```

That ordering is deliberate. If an XR render request is pending on the same queue tick, the GPU sees:

```
game scene -> menu overlay -> XR source blit
```

so the current mono OpenXR image includes the menu.

## Input

The game HWND is chained with Win32 `SetWindowSubclass`, not a destructive WndProc replacement. F7/F8/Insert/Home are reserved from vanilla window messages but still detected through `GetAsyncKeyState` by the mod. While the menu is open, mouse/keyboard/raw-input messages are consumed to reduce accidental game actions.

The OS cursor itself is not part of a D3D12 resource, so the panel texture includes a software cross at the current client-space mouse position.

## Thread-safety correction

Live menu editing means UI/input and renderer threads can touch XR mode flags simultaneously. V46.2 converts these XrManager fields to atomics:

- `diagnostic_rendering_`
- `active_vr_bridge_`
- `stereo_experiment_`
- `afr_ipd_scale_`

Existing presentation controls were already atomic.

## Matrix isolation

Starting the 50-second matrix from F8 or the menu automatically closes the menu. Menu edits are rejected while `matrix_running_for_ui_` is true. `Safe Mono Fit` can cancel an active matrix and restores temporary CryEngine CustomRes/cl_fov values.
