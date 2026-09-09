# Targeted WHGame stereo xref probe

This build replaces the slow full-process string scan with a targeted executable-code
xref scan inside `WHGame.dll`.

Known native string RVAs from the current tested build are validated first:

- `0x3DE89A0` `r_stereoScaleCoefficient`
- `0x3DE8C2A` `dual rendering`
- `0x3DE8CCD` `stereo mode`
- `0x3D26770` `r_3MonHack`
- `0x3D320F0` `r_3MonHackLeftCGFOffsetX`
- `0x3D31FD0` `r_3MonHackRightCGFOffsetX`
- `0x3DE6F28` `r_ShowRenderTarget`
- `0x3DDCE58` `r_DuplicateLastFrameOnCameraChangeWH`

The scanner walks executable memory regions of WHGame and looks for common x64
RIP-relative LEA/MOV references that resolve exactly to those addresses.

Log lines:

`STEREO-XREF target ... validation=match`
`STEREO-XREF hit target='...' code_rva=0x...`
`STEREO-XREF bytes ... data=...`

The byte dump is 64 bytes around each reference and is intended to identify function
boundaries/patterns in a disassembler.

This is still a read-only diagnostic build. It does not enable or patch stereo mode.
