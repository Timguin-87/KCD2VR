# D82 temporal upscaling: implementation and evidence

## Status

D82 implements separate DLSS and FSR instances for the two stereo eyes. This is
new source with host and native-binary verification. It is **not** a claim of a
successful Windows build, a correct headset image or improved FPS. Native remains
the established selection from the user's current run. XeSS is not implemented
for Stereo by this patch.

This implements temporal super-resolution, including the selected vendor's
native-resolution AA modes. Vendor frame generation is outside this patch.

The relevant distinction is ownership of temporal history. DLSS/FSR do not become
stereo-correct merely because native TAA already has two histories. Conversely,
rendering LEFT and RIGHT sequentially does not inherently prevent temporal
upscaling. Each eye needs its own vendor instance, matched inputs and lifecycle.

## Native interfaces verified against the supplied game

The supplied `WHGame(8).dll` has SHA-256
`bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`,
PE timestamp `0x6A350E20`, and image size `0x5B2D000`. Native entry contracts
are qualified before hooks are enabled. A different game image is refused.

| Backend | Actual game path | D82 ownership |
| --- | --- | --- |
| DLSS | WHGame exports `NVSDK_NGX_D3D12_CreateFeature`, `EvaluateFeature_C`/`EvaluateFeature`, `ReleaseFeature`, `Shutdown1` | Original feature for LEFT; a distinct feature created from the same live creation dictionary for RIGHT |
| FSR | WHGame imports the public `ffxCreateContext`, `ffxDispatch`, `ffxConfigure`, `ffxDestroyContext` loader API | Original opaque context for LEFT; a separate SDK-created context for RIGHT; native configuration mirrored |

DLSS's actual game evaluate helper tail-calls `EvaluateFeature_C`; intercepting
only `EvaluateFeature` would miss the real path. The native caller ignores the
evaluation return on its way to restoring render state. Returning an API error
therefore cannot by itself prevent stale output from reaching presentation.

The current FSR path is the public FFX loader interface, not an older FSR2
scratch-buffer layout. The native create helper is at RVA `0x1DFEC30`; the
upscale dispatch helper is at `0x1DFDFF0`. Its documented descriptor is `0x1B0`
bytes, with jitter at `0x168` and reset at `0x198`. Compile-time checks cover
these layouts. There is no opaque context copy, shared backend scratch allocation
or guessed late recreation.

The native FSR preprocessing path selects the velocity resource using the
current CRV eye selector; the supplied binary exposes the two slots at pipeline
offsets `0x858` and `0x860`. It also copies the native jitter before execution.
The synchronous SDK dispatch precedes the pipeline's final composition. These
are call/data-flow observations, not a capture of the actual GPU resource pixels.

## Creation and lifetime

Hooks install on the startup worker, outside DllMain's loader lock, before the
companion starts. They are checked again after configuration loads if the game
module was not available at the first attempt. D82 does not initialize a second
device or SDK; it intercepts native creation performed by the game.

When Stereo is requested, both supported bridges capture future native
creations, even if the currently selected technology is the other one. At an
appropriate creation call, the right-eye instance is made from that same live
descriptor chain or parameter dictionary. Its resources/history are allocated
independently by the SDK. No parameters are saved for a speculative later clone.

If native creation happened before capture, a twin is missing, allocation fails,
or capacity is exhausted, the context is not accepted for stereo. Entering
Stereo requires a captured valid pair. The menu explains when a restart is
needed to initialize the pair. Mono use of the original instance remains native.

Both bridges use bounded context/receipt storage and add no per-frame heap
allocation. Native lifecycle calls provide the destruction boundary. FSR's
native destroy path drains GPU work before calling the loader; the right context
is released at that same boundary with the same allocator. DLSS twins follow
native feature release/device shutdown. A mode toggle invalidates histories and
routing; it does not free potentially in-flight GPU objects.

Hook setup constructs trampolines before enabling wrappers. Creation is protected
by the lifecycle/evaluation transaction. A failed transaction is terminal for
that process and retains its trampoline storage, because a racing native
pass-through may still be returning. Normal shutdown disables stereo routing
while the native lifecycle hooks remain available to release SDK-owned state.

## Per-eye evaluation and history continuity

The current render-thread eye flow supplies epoch, frame, eye and pipeline
identity. A stale frame, wrong selector, unavailable prepared view or restoration
pass cannot authorize a temporal evaluation. There is no global guessed eye
fallback for unrelated worker calls.

The original LEFT handle and distinct RIGHT handle receive the game's current
color, depth, motion vectors, output, jitter, dimensions and quality settings.
For FSR the bridge copies only the documented dispatch descriptor, preserving
its resources and extension chain. For DLSS it temporarily adds Reset when
needed, verifies that write, then restores and verifies the original native
parameter. No camera projection, IPD, FOV or motion-vector scale is invented.

LEFT begins each pair by checking continuity for both eye histories. A missing
or refused RIGHT must not allow LEFT to continue with a newer history while
RIGHT resets alone. Both histories reset after menu/mono use, epoch/pipeline
changes, frame gaps or invalidated work. A native LEFT reset propagates to RIGHT.
A reset discovered only at RIGHT after an unreset LEFT is refused for that pair.
Duplicate advancement of an eye in the same frame is also refused.

The bounded completed-pair receipts identify the same epoch/frame and independent
instances. Queue-side reads use atomics, without the SDK/registry mutex, COM calls
or allocation. This avoids the lock cycle in which the submission thread waits
for an SDK call that itself is waiting for submission.

The selected technology is bound when a stereo epoch starts. A live CVar change,
including DLSS/FSR to Native, cannot turn an incomplete old pair into an accepted
new-technology pair. The existing failed-run path drains/restores the run and
rearms after its existing backoff, at which point a new epoch captures the choice.
The mod does not silently rewrite the game's AA selection.

## Presentation must use finished composition

The existing Native capture saves LEFT before the first RIGHT main-target
overwrite, then saves/publishes RIGHT after its qualified native batch. That
established path remains available for Native.

Temporal output needs a stronger join. An early RIGHT main-resource barrier
does not prove the upscaler and final composition have produced the intended
image. The temporal path preserves the early LEFT snapshot but accepts RIGHT
only from the actual native final-compose draw's command-list generation, after
both SDK evaluations have succeeded for that epoch/frame. Reset or reuse of a
command list invalidates its earlier composition tag. ClearState invalidates its
observed render-target binding, while retaining commands already recorded. A
later observed draw to the same main output outside the qualified composition
scope invalidates a pending tag. The submission check revalidates both
composition identity and the vendor receipt.

The existing stereo admission requires the native pass-scheduler setting to be
disabled. D82 also checks the actual owner, back-pointer and active/defer
predicate at the composition call. The asynchronous deferred branch is refused;
there is no assumed transfer of eye TLS into unknown worker jobs. See
TEMPORAL_COMPOSITION_D82.md for the exact native boundary and command-list tests.

This is still queue-submission evidence, not proof of completed GPU execution
or headset scanout. The existing owned texture slots, queue synchronization and
OpenXR consumer remain responsible for their original parts of presentation.
An incomplete temporal pair is withheld rather than declared ready from an API
call alone.

## User-facing choices

Stereo offers Native, DLSS and FSR. The temporal choices explicitly say they are
new D82 candidates requiring a headset test. XeSS remains available in Mono only.
DLAA and native-AA quality settings still use the selected vendor backend; they
follow the same per-eye path rather than becoming Native/off.

Startup Save and the VR Graphics page write the selected native CVar values for
the next full game restart. Keep remains a verified no-write operation; an
unknown, unreadable or unsupported current selection cannot authorize it for
Stereo. Current-backend checks prevent a stale UI from bypassing these rules.
Opening the menu or applying the source patch does not enable an upscaler.

If Mono is currently active and twins are missing, a supported Stereo request
is still saved for restart. Live activation remains gated until the required
pair exists. This lets the next launch capture the native creation calls rather
than leaving the saved mode stuck in Mono.

## Verification and unresolved acceptance work

See VALIDATION_D82.md and the compact result files accompanying the patch. The
runtime fixtures use the actual production routing code with mock SDK histories;
the tests check distinct instances, descriptors, reset continuity, missed eyes,
lifecycle, queue-safe receipts, menu behavior and old-defect negative controls.
Native-binary work verifies the ABI and relevant call paths separately.

Neither can prove the GPU contents of color/depth/motion vectors, vendor shader
behavior on the user's driver, final image quality, stereo comfort, VRAM cost or
FPS. Two independent temporal instances necessarily add history storage relative
to one shared instance. Any performance benefit depends on the game's actual GPU
work and the selected quality. No benchmark improvement is claimed.

The next acceptance boundary is a normal Windows build with the exact D82 marker,
then an in-game/headset comparison of Native and one supported temporal choice.
The existing unified log records installation, captured pairs and evaluated
epochs/frames; this patch does not add another always-on recorder or sidecar.

## Primary API references and license

- NVIDIA DLSS Programming Guide, multiple-view/VR instances:
  https://raw.githubusercontent.com/NVIDIA/DLSS/main/doc/DLSS_Programming_Guide_Release.pdf
- NVIDIA public NGX creation/evaluation/release declarations:
  https://github.com/NVIDIA/DLSS/blob/main/include/nvsdk_ngx.h
- AMD FidelityFX API lifecycle and descriptor-chain documentation:
  https://gpuopen.com/manuals/fidelityfx_sdk/getting-started/ffx-api/
- Pinned AMD public headers used here, SDK v1.1.3:
  https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/tree/54fbaafdc34716811751bea5032700e78f5a0f33/ffx-api/include/ffx_api

The MIT license, upstream hashes and the one consumer-export-macro adjustment
are preserved in `deps/FidelityFX_API_1_1_3`. No game or vendor runtime DLL is
included in the source patch.
