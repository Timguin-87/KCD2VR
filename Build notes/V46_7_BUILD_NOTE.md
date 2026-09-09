# V46.7 build note

Use the normal Windows build:

```bat
rmdir /s /q build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=E:/backup/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH=E:/backup/DetourModKit
cmake --build build --config RelWithDebInfo
```

Static V46.7 validation: 20/20 PASS.
Linux-side CMake parsing reaches the expected missing DetourModKit dependency; no Windows ASI binary is produced in this environment.
