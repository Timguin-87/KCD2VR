# V46 Source Build Note

This package is source/project only. It was statically/logic-tested in the packaging environment but not compiled as a Windows `.asi` here.

Use the same Windows/MSVC + DetourModKit + OpenXR configuration as the previous working builds. Regenerate the CMake build tree so all V46 translation units are present.

Recommended clean build pattern:

```bat
rmdir /s /q build
cmake -S . -B build <your usual CMake arguments>
cmake --build build --config RelWithDebInfo
```

At runtime the safe packaged defaults are mono Fit, stereo experiment off, IPD experiment off, swap off. Load gameplay and press F8 once to run the 7 x 5-second matrix.
