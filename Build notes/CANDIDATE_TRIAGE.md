# COMMON-DISPATCH-CANDIDATE triage

## Background

`THEORY_SWEEP.md` (Phase 3: "Dormant 3Mon code leads to a common view/render
dispatcher") ran a 2026-09-01/09-02 session for ~20 hours across 26 launches,
13 successful device captures, and produced **zero errors/warnings/crashes**
in the entire log - the most stable run of this project at that point. Full
log: `KCD2VR-debug.log`.

Phase 3 found the 3MonHack-related caller region twice ("CallerA" and
"CallerB" - two known code sites that reference the r_3MonHack CVar / its
offset settings), enumerated every direct call target from each, and logged
whichever targets appear in BOTH callers' call sets. 26 such shared targets
were found. A function called by both regions is a real, concrete candidate
for a shared dispatch point they route through - possibly (not confirmed)
a surviving multi-view/stereo render path.

**Cross-checked against the sweep's other phases (Phase 1 stereo-help
strings, Phase 2 backing storage, Phase 4 secondary-view strings): no exact
or near-adjacent RVA overlap with any of the 26 candidates below.** These
were not independently corroborated by the string search - they need actual
disassembly to evaluate, not just cross-reference.

## The 26 candidate addresses (WHGame.dll RVAs, deduplicated)

```
0x1AB52D0   0x1AB52E8   0x1DF9910   0x254CE1C   0x254DE70
0x2557764   0x25582BC   0x255900C   0x255905C   0x3A45B0
0x42ABDC    0x42ACEC    0x4EC140    0x4F66CC    0x4F75C0
0x4FC624    0x57B510    0x57E130    0x7761C4    0x7761E0
0x78FC2C    0x78FE10    0x78FE28    0x7CA3FC    0x9B8184
```

## UPDATE: static disassembly performed directly (objdump), real findings

Claude had direct access to the actual `WHGame.dll` binary and ran targeted
`objdump` disassembly (Intel syntax) around each of the 26 candidates - no
IDA/Ghidra available, so no decompilation/RTTI recovery, but real
instruction-level analysis was possible. Full methodology: disassemble a
~0x180-byte window starting at each candidate's VA (`ImageBase 0x180000000`
+ RVA), confirm the candidate lands on a genuine instruction boundary (all
26 do - confirms the sweep's own resolution logic is sound), then score each
for XMM/vector instruction density, vtable-style indirect calls, and direct
calls, as a proxy for "does this look like camera/view/matrix work."

**Two candidates stand out sharply above the rest** (everything else scored
under 10 XMM instructions in the same window; these two scored 26 and 41):

### `0x254DE70` - likely candidate: writes a 4x4 matrix into per-eye(?) storage

Copies four consecutive 16-byte blocks (`movups XMMWORD`, i.e. a full 4x4
float matrix) from a source object at `[rax+0x9c80..0x9cb0]` into a
destination object at `[rsi+0x1820], [+0x1830], [+0x1840], [+0x1850]`.
Afterward, contains TWO nearly-identical blocks of scalar float math (same
two constants at `0x18409a210`/`0x18409a488` multiplied together, then
divided into another value), separated by a conditional branch
(`test eax,eax` / `jne`) - consistent with, though not confirmed as, a
per-eye (left/right) branch computing slightly different derived values
from the same copied matrix.

### `0x2557764` - likely candidate: reads that same matrix, does per-eye-shaped math

Reads the SAME four fields (`[rbx+0x1820/0x1830/0x1840/0x1850]`) that
`0x254DE70` writes - a real producer/consumer relationship between these
two functions, operating on the same object layout. Heavy `shufps`/
`mulss`/`addss` (vector component broadcast + dot-product-shaped math) on
those values, plus one vtable call (`call QWORD PTR [rax+0x38]`, i.e. index
7 of some COM-like interface) and one direct call.

### What this means and doesn't mean

This is real, concrete evidence of two functions operating on a shared
object with a 4x4-matrix-shaped field at offset `0x1820`, with math shaped
like per-eye/asymmetric projection derivation. It is genuinely the
strongest lead among the 26. **It is not confirmed to be stereo/VR-specific
code** - the same shape (copy a matrix, derive per-branch scalar values)
could also just be ordinary single-camera projection setup that happens to
look similar for mundane reasons. Confirming this needs a real decompiler
(Ghidra/IDA) with type/RTTI recovery to identify the actual struct these
offsets belong to, and to see what calls INTO these two functions and from
where.

**Update, later session**: subsequent live investigation (see
`CRENDERVIEW_CHECKOUT_STATE_SAFE_V4.md`/`V5`/`NATIVE_SELECTED_CAMERA_PAIR_V10.md`)
went a different, complementary direction - live-instrumented observation of
`CRenderView` checkout/construction rather than deeper static analysis of
these specific 26 addresses. That live path found a genuine indexed
left/right-style pair selector at `WHGame+0x865550`/`0x866F6C`
(`CRenderView+0x8EE8`/`+0x9508`, `index = ... & 1`, 0x310 stride) -
worth noting this is a DIFFERENT offset region than the `+0x1820`-`+0x1850`
region `0x254DE70`/`0x2557764` operate on. Whether these two findings are
related (e.g. one feeds the other) or independent has not been confirmed.

## Triage criteria, for whoever has IDA/Ghidra/binary access (remaining 24)

1. **Function size.** A genuine "dispatch a secondary/stereo view" function
   has to do real work. A one-line thunk or trivial getter/setter is
   almost certainly not it.
2. **Proximity clustering.** Several pairs sit very close together and are
   worth checking as a group:
   - `0x1AB52D0` / `0x1AB52E8` (0x18 apart)
   - `0x255900C` / `0x255905C` (0x50 apart)
   - `0x42ABDC` / `0x42ACEC` (0x110 apart)
   - `0x7761C4` / `0x7761E0` (0x1C apart)
   - `0x78FC2C` / `0x78FE10` / `0x78FE28` (within ~0x1FC of each other)
3. **Call graph fan-out.** A real view-dispatch function likely calls INTO
   other rendering-related code, not just memory/string/logging utilities.
4. **RTTI/vtable proximity** to known rendering-related class names.
5. **Cross-check against already-resolved anchors** from this project
   (camera_hook.cpp's frustum builder, the CRenderView-related addresses
   found in the later live-investigation series).

## Do not, on the basis of this document alone

- Do not call, hook, or jump to any of the remaining 24 unverified addresses.
  They are unconfirmed candidates, not confirmed functions.
- Do not assume any finding here removes the need for `DiagnosticRendering`'s
  existing safety default (`false`) or `EnableNativeEyeWrite`'s safety
  default (`false`) - both gate real risk categories this project has
  learned about the hard way (see ROADMAP.md).
