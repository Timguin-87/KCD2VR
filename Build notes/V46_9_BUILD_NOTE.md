# V46.9 build note

Fresh build:

```bat
rmdir /s /q build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=E:/backup/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH=E:/backup/DetourModKit
cmake --build build --config RelWithDebInfo
```

V46.9 source changes are limited to the existing safe bridge/menu/control paths plus read-only frame identity instrumentation. No retired camera-pair write or unverified renderer re-entry is re-enabled.
