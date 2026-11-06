# Dependencies and notices

The hook library and its headers are under `deps/DetourModKit`. Keep its original
acknowledgements and license notices with redistributed source or binaries.
The capacity patch and pinned dependency manifest describe the local variant;
compression does not change its library bytes.

OpenXR is obtained from the revision pinned by CMake, or an explicitly supplied
source checkout. Its upstream notices remain with that dependency.

Game-specific interfaces are checked against the supported retail binary. A game
DLL is not distributed here. Runtime fingerprints and native-byte qualification
are part of the implementation, not a reason to distribute the game itself.

Project documentation has been condensed for this source release. Required
third-party notices have not been rewritten into personal diary entries or
removed as part of that cleanup.

## FidelityFX API headers used by D82

`deps/FidelityFX_API_1_1_3` contains MIT-licensed public API headers and their
original license from AMD's FidelityFX SDK v1.1.3, commit
`54fbaafdc34716811751bea5032700e78f5a0f33`. PROVENANCE.json records each upstream
URL/hash and the one documented consumer-export-macro adjustment. The layouts
are checked against the supplied game's actual FFX creation/dispatch calls.
No AMD, NVIDIA or game runtime DLL is redistributed by this patch.

The DLSS bridge uses the game's exported NGX API entry points, checked against
the exact supported retail image. It does not access private nvngx_dlss internals
or introduce a separate NGX initialization. NVIDIA's guide remains the public
contract for independent per-view feature instances.
