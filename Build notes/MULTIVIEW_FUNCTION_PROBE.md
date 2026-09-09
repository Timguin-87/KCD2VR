# KCD2 multiview function probe

This build focuses on the runtime-looking `r_3MonHack` code reference discovered at:

`WHGame.dll + 0x254DEA7`

It does not scan the whole process.

At startup it:

1. validates the expected `48 8D 15` instruction at the known RVA;
2. finds nearby `RET` boundaries as a rough function-boundary hint;
3. dumps a raw byte window from `center - 0x180` through `center + 0x280`;
4. scans a wider `center - 0x400` through `center + 0x800` range and resolves:
   - direct `CALL rel32`;
   - direct `JMP rel32`;
   - common RIP-relative `LEA` / `MOV` / `CMP`;
   - RIP-relative `MOVSS` scalar-float loads;
5. previews printable string targets when a resolved address points to ASCII.

Useful log prefixes:

`MULTIVIEW-PROBE begin`
`MULTIVIEW-PROBE nearest-RET`
`MULTIVIEW-PROBE bytes`
`MULTIVIEW-PROBE CALL-rel32`
`MULTIVIEW-PROBE LEA-rip`
`MULTIVIEW-PROBE MOVSS-rip`
`MULTIVIEW-PROBE target-string`

This remains read-only. It does not enable `r_3MonHack` or patch WHGame.
