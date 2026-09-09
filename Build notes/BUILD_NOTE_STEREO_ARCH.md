# Stereo archaeology build note

The stereo archaeology scanner is header-only in this build.

This avoids an unresolved external if an existing Visual Studio/CMake build directory
has a stale source list that does not compile `stereo_archaeology.cpp`.

Recommended clean rebuild anyway:

    cmake -S . -B build <your normal configure options>
    cmake --build build --config RelWithDebInfo

If you use an existing build directory, deleting `build` and reconfiguring is the
safest way to ensure CMake project files match the current source tree.
