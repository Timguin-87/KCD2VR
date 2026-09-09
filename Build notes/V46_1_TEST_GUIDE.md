# V46.1 test guide

1. Build after regenerating the CMake project if needed.
2. Use the included `KCD2VR.ini`; no `autoexec.cfg` or `user.cfg` is needed.
3. Load normal gameplay. A place with visible water/reflections is preferred for the final Recursive View phase.
4. Press **F8 once**.
5. Do not press Insert/Home during the 50-second cycle.
6. Let `V46.1-MATRIX-COMPLETE` appear, then send the log.

Useful visual notes only:
- which of baseline Fit / CustomRes / CustomRes+FOV looks least zoomed and most natural;
- whether the red-left / green-right markers exchange physical eyes in the swapped phase.

The matrix restores original CryEngine CustomRes/cl_fov values and returns to mono Fit automatically.
