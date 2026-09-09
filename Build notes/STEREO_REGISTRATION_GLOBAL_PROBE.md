# Stereo registration/global probe

Target build: `release_1_5_1308617_856`

This probe starts from the confirmed executable reference to:

- `r_stereoScaleCoefficient` at `WHGame + 0x10178D6`

The previous table probe showed:
- direct xrefs exist for the CVar name,
- `"dual rendering"` / `"stereo mode"` have no direct code xrefs,
- no ordinary 64-bit pointer table to those strings was found.

This build therefore reverses the registration layer instead of the string table.

It is read-only.

## What it logs

`STEREO-REG`
- raw bytes from `0x1017600 .. 0x1017B80`
- every detected `CALL rel32`
- every RIP-relative `LEA` / `MOV`
- every RIP-relative `MOVSS`
- printable strings reached by those references

`STEREO-GLOBAL`
- non-string RIP-relative targets collected from that registration block
- executable xrefs elsewhere in WHGame to those targets
- local code context around every hit

The intended discovery chain is:

`r_stereoScaleCoefficient name`
→ registration call
→ backing global/storage
→ code that reads/tests the global
→ renderer/stereo implementation

Use `DiagnosticRendering=false`.
