# Stereo / dual-render table probe

This build pivots away from `r_3MonHack` and targets the surviving CryEngine-style
stereo cluster in `WHGame.dll`.

Known string RVAs for `release_1_5_1308617_856`:

- `r_stereoScaleCoefficient` -> `0x3DE89A0`
- `dual rendering` -> `0x3DE8C2A`
- `stereo mode` -> `0x3DE8CCD`

The probe is read-only.

It performs four passes:

1. dumps ~0x600 bytes around the stereo string cluster;
2. scans executable WHGame pages for direct RIP-relative xrefs to each string;
3. scans readable WHGame image memory for **64-bit pointers** to each string;
4. for every pointer/table slot found, scans executable code again for RIP-relative
   references to that pointer slot and dumps context.

The new second-stage path is intended to discover enum/help/registration tables that
indirectly reference `"dual rendering"` / `"stereo mode"`, then trace those tables
back into live renderer code.

Useful log prefixes:

- `STEREO-TABLE ptr-hit`
- `STEREO-TABLE code-xref`
- `STEREO-TABLE code-xref-context`
- `STEREO-TABLE second-stage complete`

Use `DiagnosticRendering=false`.
