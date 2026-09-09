# KCD2VR V61.3 — Native stereo storage-watch diagnostic

Purpose: test the renderer-consumed backing values first, before touching FlowGraph ProcessEvent or enabling native stereo.

## Static result already established
A full-image RIP-relative displacement scan of WHGame.dll found exactly one direct code reference to each retained stereo backing float, all in the CVar registration block around WHGame+0x1017900:
- r_StereoEyeDist: WHGame+0x50DBDCC
- r_StereoScreenDist: WHGame+0x50DBDC8
- r_StereoHudScreenDist: WHGame+0x50DBDC0

Therefore any real renderer consumer must reach these values indirectly (for example through an ICVar/storage pointer), or the path is dead.

## Runtime test
F8 (or the menu's Stereo Test button) now starts a 10-second diagnostic window. The old AFR/DUAL/pseudo stereo ladder is unreachable in this build.

The diagnostic installs x64 hardware data breakpoints on all process threads for the exact 4-byte backing floats. It is read-only: it never changes the three values.

Expected log markers:
- V61.3-NATIVE-STORAGE-WATCH ARMED
- V61.3-NATIVE-STORAGE-HIT ... ripAfter=WHGame+0x...
- V61.3-NATIVE-STORAGE-SUMMARY ...
- V61.3-NATIVE-STORAGE-DONE ... conclusion=LIVE_CONSUMER_FOUND / NO_RUNTIME_READS_OBSERVED

Test in ordinary gameplay. Do not change stereo CVars during the 10-second window. Turn/look/move normally so camera/render code executes.

If LIVE_CONSUMER_FOUND: static-disassemble each reported consumer RIP outward toward eye-index / StereoL/StereoR creation/binding. Only then add FlowGraph observers.
If NO_RUNTIME_READS_OBSERVED: do not spend more time on FlowGraph plumbing; treat the retained native dual-render path as likely dead unless another indirect storage mechanism is identified.

Safety: the probe skips threads that already have enabled hardware breakpoints (Dr7 low enable bits nonzero), to avoid clobbering a debugger/user watchpoint.
