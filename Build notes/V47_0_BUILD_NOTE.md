# V47.0 build note

Fresh Windows build:

```bat
rmdir /s /q build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=E:/backup/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH=E:/backup/DetourModKit
cmake --build build --config RelWithDebInfo
```

Linux-side validation cannot link/configure DetourModKit. The V47.0 source-specific static validator passes 20/20 and CMake parses until the expected missing DetourModKit package.
