# V46.1 WHGame.dll static evidence

Build basis: the provided current `WHGame.dll` (`release_1_5_1308617_856` lineage; all RVAs are version-sensitive).

## CryEngine console / CVar calls

The active `r_Width` / `r_Height` runtime update path around `WHGame+0x245F33D` does:

1. obtain the console object;
2. call console vslot `+0xB8` with the CVar name;
3. call returned ICVar vslot `+0x38` with the integer width/height.

Observed generic ICVar accessors/setters in the same binary:

- integer getter `+0x10`
- float getter `+0x20`
- integer setter `+0x38`
- float setter `+0x40`

The process-global console pointer used by these paths is at `WHGame+0x492D8A8`.

## CustomRes registration

String/xref RVAs:

- `r_CustomResWidth` string `0x3DE8DF0`, registration xref near `0x1017A55`
- `r_CustomResHeight` string `0x3DE8EB0`, registration xref near `0x1017A82`
- `r_CustomResPreview` string `0x3DE8F18`, registration xref near `0x1017AA9`
- `r_CustomResMaxSize` string `0x3DE9168`, registration xref near `0x1017A28`

The `r_CustomResMaxSize` registration loads `0x1000` (4096) as its default. V46.1's automatic targets (1248×1344 and 1872×2016) are deliberately below that limit.

## FOV

`cl_fov` string RVA is `0x3A6F9A8`. KCD2 retrieves it through the same console `+0xB8` path. Historical live camera summaries in this project established `CCamera float[12] / +0x30` as the symmetric FOV-like field (typically ~1.309 rad / 75°), so V46.1 observes that field while changing `cl_fov` instead of treating CVar readback as proof.
