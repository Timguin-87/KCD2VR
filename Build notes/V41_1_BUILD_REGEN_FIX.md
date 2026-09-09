# V41.1 build regeneration fix

The V41 source package already contains `src/frame_sync.cpp`, and `CMakeLists.txt` lists it in `add_library(KCD2VR SHARED ...)`.

The reported LNK2019 errors mean the generated Visual Studio project in `build/` is stale and did not compile/link `frame_sync.cpp`.

## Fix
Delete/regenerate the build tree after extracting this package:

```bat
rmdir /s /q build
cmake -S . -B build <your usual DetourModKit/OpenXR CMake arguments>
cmake --build build --config RelWithDebInfo
```

Or run `build_release.bat` and append your usual CMake cache arguments if needed.

After configure, verify the generated project contains `src\\frame_sync.cpp` before building.
