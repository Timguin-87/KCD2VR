# D82: publishing the completed temporal stereo image

## What was wrong

The established Native path preserves LEFT before the first RIGHT batch that touches the engine's main render target, then preserves RIGHT after that batch. That boundary remains the Native0 behavior.

For a temporal upscaler, a main-target transition is insufficient proof that the same eye's upscaling and final composition have finished. The first D82 integration checked the SDK receipts at this early boundary, after already consuming the once-only RIGHT copy. An early barrier could therefore spend the copy before a later, valid final-composition batch arrived. Merely making `PairReady` lock-free avoided a lock cycle but did not fix the boundary.

## The repaired ownership chain

LEFT is still copied immediately before the first RIGHT main-writer batch. Temporal RIGHT follows a separate completion certificate:

1. The current native `FinalCompose` renderer and pipeline must match the active eye flow. The shared composition helper must be called from the verified call at `WHGame+0x864654`, whose return address is `+0x864659`.
2. The current RIGHT camera/selector/frame/pipeline must pass the existing TLS view qualification. The input/output textures must be readable and valid. The native scheduler ownership and deferred-pass predicate are re-read at this exact call.
3. Both SDK evaluations must already have completed successfully for the same stereo epoch and frame. This opens a small TLS composition scope; the bootstrap diagnostic trace is not used as production evidence.
4. A real `DrawInstanced` or `DrawIndexedInstanced` must return inside that scope, while its current command-list recording has the observed engine-main RTV bound. Cached native bindings are supported: the bind need not be repeated inside the helper.
5. Submission must contain that exact command-list recording generation. The queue snapshots its certificate before the normal archive consumes the recording, submits the game's original batch, and rechecks the certificate's epoch/frame SDK receipts and output identity before claiming RIGHT.

An earlier generic barrier cannot consume RIGHT. Missing scope, failed reads, an unsupported deferred path, missing SDK success, another output, another frame, another epoch, or another list recording withholds publication.

## Recording and hook lifetime

The public D3D12 command-list `Reset` method is COM slot 10. It invalidates the temporal binding and composition certificate and advances the recording generation. The public `ClearState` method is slot 11. It clears the binding while preserving commands already recorded. Neither observer changes the game's arguments or native return value.

Both observers prepare their original function pointers before enabling either hook. A partial enable failure disables both and retains their trampoline/original storage until process teardown. Shutdown similarly stops temporal admission and retains the storage, because a command-list callback need not share the render callback's lifetime.

A later observed draw into the same main target, outside the verified composition scope, invalidates the pending composition certificate. An atomic flag keeps this registry work inactive before composition and in ordinary Native0 use. Non-main RTV observations clear existing state without allocating tracking entries. The registry remains bounded by the existing 384-list capacity.

These rules cover the known native recording/submission path. They are not a general guarantee against arbitrary third-party GPU writes or command-queue changes.

## Native evidence and supported path

The supplied `WHGame(8).dll` has SHA256 `bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7` and PE timestamp `0x6A350E20`.

Its pipeline calls Standard before FinalCompose. Standard reaches the native super-resolution dispatcher and its FSR/DLSS evaluation wrappers before returning to FinalCompose. The exact final-composition helper at `+0x2543A6C` passes the input to the native texture binding path, passes the output to its RTV setter, and executes the full-screen draw pass. The synchronous pass reaches actual D3D12 OM/Draw calls before the helper returns. Existing prologue witnesses and the helper's 510-byte FNV1a range hash are checked; D82 adds exact callsite and scheduler-predicate witnesses.

The alternate pass-scheduler branch enqueues a pass record and dependencies without doing the synchronous draw. It is deliberately unsupported by this TLS certificate. The supplied game's `r_GraphicsPipelinePassScheduler` default is zero, and the existing stereo admission requires the cached scheduler setting to be zero. The fresh D82 guard also checks the actual current owner/back-pointer and active/defer conjunction so a stale checkpoint cannot authorize an asynchronously deferred call. No TLS eye is carried into unknown worker jobs.

Native FSR motion-vector preparation selects the CRV eye's velocity resource from the existing distinct `pipeline+0x858/+0x860` banks; its jitter comes through the existing native per-call stage data. D82 does not replace those descriptors or the established camera/TAA path. Correct GPU contents and headset fusion still require a game run.

## Verification

Run from the source root:

```text
python tools/check_temporal_compose_d82.py --output <evidence-directory> --native-dll <supplied-WHGame.dll>
```

The final checks exercise the production policy header and functions extracted from the actual `device_hook.cpp` and native scope qualifier:

- 49 policy checks and 85 production adapter checks, each passing in debug, optimized, and AddressSanitizer/UndefinedBehaviorSanitizer builds.
- An early RIGHT main barrier leaves the copy unspent; a later exact composition list publishes even without another generic main barrier.
- Wrong queue/list/output/frame/epoch, missing receipt, consumed-list reuse, Reset, ClearState, fresh scheduler ownership, and later main-target draws are covered.
- Six negative controls restore the defects independently: no Reset invalidation, generic-barrier claiming, no SDK receipt, no ClearState binding reset, a later main draw retaining the stamp, and no generation comparison. All are rejected by the same behavioral tests.
- Microsoft's pinned DirectX-Headers source independently confirms slots 10/11 and Draw slots 12/13. An optional Windows SDK ABI assertion is in `tests/command_list_reset_abi_d82.cpp`.

The tests do not execute the Windows detours, vendor GPU shaders, or the headset compositor. They demonstrate the repaired CPU ownership/publication contract, not measured image quality or frame rate. A queued stereo pair is not described as GPU-complete.
