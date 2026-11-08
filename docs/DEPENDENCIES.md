# Dependencies and notices

all of these are in the tree or fetched by the build, you don't install anything extra except the build tools
(BUILDING.md).

## DetourModKit

the hook library by tkhquang, under `deps/DetourModKit` (headers, the static lib as a zip, cmake config). keep its
acknowledgements and license notices with any redistributed source or binary. `DEPENDENCY_MANIFEST.json` hashes the
headers and the lib of the pinned package. the local variant has one change, `CAPACITY128.patch` (mid hook adapter capacity 64 ->
128), described in the manifest. the zip does not change the library bytes, the build restores them exactly and checks
the hash.

## OpenXR

the OpenXR loader comes from the OpenXR-SDK-Source release pinned in CMakeLists.txt (downloaded on the first configure),
or from a checkout you give with `-OpenXrSdkSource`. its upstream notices stay with it. `tests/openxr_sdk/` has the
headers for the host tests.

## FidelityFX API headers

`deps/FidelityFX_API_1_1_3` has the MIT-licensed public API headers and the original license from AMD's FidelityFX
SDK v1.1.3, commit `54fbaafdc34716811751bea5032700e78f5a0f33`. PROVENANCE.json records each upstream URL/hash and the
one consumer-export-macro adjustment. the layouts are checked against the game's own FFX creation/dispatch calls.
no AMD, NVIDIA or game runtime DLL is redistributed.

## DLSS

the DLSS bridge uses the game's exported NGX API entry points, checked against the supported retail build. it does
not touch private nvngx_dlss internals and does not do its own NGX initialization. NVIDIA's guide is the public
contract for one feature instance per view.

## the game

game interfaces are checked against the supported retail WHGame.dll (TimeDateStamp 0x6A350E20). no game file is
distributed here. the byte pins and runtime fingerprints are part of the implementation - when a pin does not match
(other game version) the feature does not run and the log says why.

## credits

libKCD2 and KCD2Tools for the game research. Crysis VR, PLANCK, HIGGS and FRIK for the VR ideas.
