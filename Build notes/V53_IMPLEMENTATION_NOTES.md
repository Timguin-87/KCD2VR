# V53 implementation notes

## Tracking architecture under test
`xrWaitFrame` remains exclusively on the XR worker thread. V53 does **not** move frame ownership or swapchain calls to the game thread.

For synchronous phases only, the KCD2 `PublishViewCamera` hook asks `XrManager` to run `xrLocateSpace` at an explicit target `XrTime`. Those calls are serialized through the existing `pose_locate_mutex_`, including against the XR thread's normal pose lookup.

The latest `predictedDisplayTime` is atomically published by `RunFrame()` immediately after `xrWaitFrame`.

## Rotation order
Current V51/V52 behavior is preserved as order 0: base KCD2 camera basis multiplied by the local recentered HMD delta (`B * D`).

Order 1 is an intentionally diagnostic opposite composition (`D * B`) in Cry coordinates. It is not assumed correct; the test exists to determine whether the existing local/world order is implicated.

## Why phase 4 remains
The earlier V48/V50 late-pose path suggested QPC-now may be worse. V53 keeps one clean synchronous-now phase so it can be compared directly against synchronous predicted-time sampling in the same build, without FOV/FPS/stereo changes.

## Prediction offsets
The +5 ms and +15 ms phases are diagnostics, not final tuning values. They test whether the image benefits from a pose targeted farther into the expected camera→display pipeline.
