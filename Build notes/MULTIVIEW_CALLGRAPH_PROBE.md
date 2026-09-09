# Multiview callgraph probe

Previous runtime evidence established a compact function at:

- start: `WHGame + 0x254DE70`
- return: `WHGame + 0x254E061`

That function directly references:

- `r_3MonHack`
- `r_3MonHackHUDFOVX`
- `r_3MonHackHUDFOVY`

and performs substantial SIMD/float work.

This build therefore:

1. dumps the complete ~0x1F1-byte function;
2. resolves its direct calls/jumps and common RIP-relative LEA/MOV references;
3. dumps the repeatedly-called helper at `WHGame + 0x1D9409D`;
4. scans executable WHGame memory for **direct CALLs to `0x254DE70`**;
5. dumps context around every caller.

The purpose is to identify the owning renderer/view dispatch path before we hook or
modify it. This build remains read-only.
