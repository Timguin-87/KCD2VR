# Building on Windows

Use Visual Studio 2022 with the C++ desktop workload, CMake and Python 3. A normal
x64 Windows build is the release check. Portable tests alone are not a DLL build.

Run from the extracted source root:

```bat
build_release.bat
```

It writes the DLL and main companion into `dist/prerelease`. Nothing is copied
into the game. Build logs and intermediate files stay outside the release ZIP.

For a short explicit build path:

```bat
build_release.bat -BuildDirectory E:\kcd2vr-build\local
```

The script also chooses a shorter path automatically when a deeply nested source
directory would exceed the native build tools' path limits.

## Dependencies

The bundled hook dependency is pinned. Its static library is stored as a ZIP so
the source tree does not contain a single oversized library file. The build
restores the exact original bytes under its dependency directory and checks the
hash. A direct CMake dependency override uses `DetourModKit_DIR`; a generic
`CMAKE_PREFIX_PATH` alone does not override the pinned package. Do not replace it with a similarly named library from an older build.

OpenXR retains its pinned source dependency. To use an existing SDK source tree:

```bat
build_release.bat -OpenXrSdkSource E:\deps\OpenXR-SDK-Source
```

Otherwise CMake can fetch the pinned revision during its first configure. Python
can be selected explicitly with the `KCD2VR_PYTHON` environment variable. These
scripts do not install compilers or Python packages automatically.

## Companion and optional gore

The main companion is built from its gameplay-only base plus the checked-in
overlays. The optional gore PAK is not an input to a normal DLL/main build.

If a separate optional gore distribution is available, it can be staged alongside
the normal output explicitly:

```bat
build_release.bat -OptionalGoreSource E:\downloads\KCD2VR_OPTIONAL_GORE.zip
```

Direct CMake uses `KCD2VR_OPTIONAL_GORE_SOURCE` for the same optional input. Do not
merge that PAK into the main one. Use a clean output directory when switching
between package layouts; an unexpected old optional directory is reported rather
than silently included or deleted.

## Checks

The `tests` directory contains portable policies and fixtures that exercise the
actual adapters with controlled engine calls. Several tools extract the current
production functions before compiling their fixtures. Keep generated output in
a separate build directory.

Useful focused runners live in `tools`. Read their `--help` for prerequisites.
Some require GCC/Clang; Windows-native checks use MSVC, and real SEH tests must use
the Windows SDK rather than a lightweight Windows shim. A C++ exception substitute
does not verify Windows structured-exception cleanup.

The release validation report separates:

- Source inspection and exact native-byte checks.
- Host tests, including which engine calls were replaced.
- A completed Windows DLL link.
- Actual headset observations.

Keep failures and unavailable checks visible. Changing an old fixture until it
passes is not useful if it no longer exercises the current code.

## Source archive and Git

The clean archive includes source, tests, required assets, dependency notices and
build inputs. It excludes `.git`, build caches, object files, symbols, generated
DLLs, old deliveries and private research/log collections. The large hook library
is compressed; the optional gore package is separate. To make another clean
archive, run `python tools/export_source.py --zip KCD2VR_SOURCE.zip`.

If uploading the source to a new repository, start from the clean archive rather
than importing the old private repository history. Old combined PAKs still exist
in that history even after they are removed from the current tree. That preserves
the investigation locally without making every checkout download it.
