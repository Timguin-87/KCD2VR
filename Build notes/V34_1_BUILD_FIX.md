# V34.1 build fix

Fixes MSVC C2712 in `InstallStereoGateCandidateV34()`.

Cause: the V34 installer used `__try/__except` in the same function that later constructs/moves DetourModKit C++ objects requiring stack unwinding. MSVC forbids SEH `__try` in such a function.

Fix: unsafe raw pointer dereferences were moved into `ResolveStereoGateCandidateV34()`, a trivial SEH-only helper. The installer now performs ordinary C++ hook construction outside SEH. Probe behavior is otherwise unchanged and remains read-only.
