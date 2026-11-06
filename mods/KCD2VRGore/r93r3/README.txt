Optional gore source only — based on exact62.39-D77 assets.
zzkcd2vr_gore.lua is the canonical mapped source for the separate optional PAK.
No assets or PAK are included here. D77_DONOR.json qualifies the preserved donor.
The existing donor D74 receipt is stale; tools/optional_gore_r93.py checks the full
known PAK/manifest hashes before replacing only the Lua and issuing a new receipt.
Use tools/build_release.ps1 -OptionalGoreSource <the D77 donor folder> for an explicit
later optional build. Main-only builds remain gore-free. Nothing is installed.
The native lvl1 handover and this Lua must be used together; older Lua causes exact
native cut-batch rollback. Head-only cap/helmet skins may follow the detached head;
coif/cowl/hood and neck-covering layers remain on the body intentionally.
