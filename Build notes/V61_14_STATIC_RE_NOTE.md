# V61.14 static RE note

No gameplay code changed in this branch. V61.13.1 behavior is preserved exactly.

Added `KCD2_STEREO_INTERFACE_RE_V61_14.md`, which records the new UEVR-style stereo-interface reverse-engineering results against the supplied current `WHGame.dll`.

The key result is that current KCD2 retains the stereo **parameter** layer but the historic public stereo **activation/output** CVars are absent. The next target is the current renderer vtable / stereo object interface, not another CVar test.
