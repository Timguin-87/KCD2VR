# DetourModKit installed-package ABI record (generated at configure time).
#
# Machine-readable facts about the toolchain that produced this prefix. The archives here are C++ static
# libraries with no extern "C" boundary, so these axes determine whether a consumer is ABI-compatible. This
# file only records the facts; compatibility policy belongs to the package-validation layer.
set(DetourModKit_ABI_COMPILER_ID "MSVC")
set(DetourModKit_ABI_COMPILER_VERSION "19.44.35228.0")
set(DetourModKit_ABI_SIMULATE_ID "")
set(DetourModKit_ABI_STL "msvc-stl")
set(DetourModKit_ABI_CXX_STANDARD "23")

# MSVC CRT selection genex (empty on non-MSVC toolchains); the consumer's effective CRT must agree per config.
set(DetourModKit_ABI_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# "default": the archives never override _ITERATOR_DEBUG_LEVEL, so they sit at the STL's own level for each
# configuration and a stock consumer matches without any exported macro.
set(DetourModKit_ABI_ITERATOR_DEBUG_LEVEL "default")

set(DetourModKit_ABI_SYSTEM "Windows")
set(DetourModKit_ABI_ARCHITECTURE "x64")
set(DetourModKit_ABI_POINTER_SIZE "8")

# Configuration of the producing tree: "multi-config" when the producer generator holds several,
# "unspecified" when a single-config producer set no build type.
set(DetourModKit_ABI_BUILD_TYPE "Release")
