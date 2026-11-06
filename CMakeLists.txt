cmake_minimum_required(VERSION 3.20)
project(KCD2VR LANGUAGES CXX)

# Data-only lane source lists are validated before CMake adds any native TU.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_subsystems.py" COMMAND_ERROR_IS_FATAL ANY)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Recipe edits must be compiled explicitly, never silently ignored by WHGame.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS data/VRBodyCombos.xml src/subsystems/masterstrike_combos/body_recipes.h)
file(SHA256 "${CMAKE_CURRENT_SOURCE_DIR}/data/VRBodyCombos.xml" BODY_COMBO_SHA)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/masterstrike_combos/body_recipes.h" BODY_COMBO_ID REGEX "sourceSha256=")
if(NOT BODY_COMBO_ID MATCHES "${BODY_COMBO_SHA}")
    message(FATAL_ERROR "VR body recipes changed: run python tools/compile_body_combos59c7g.py before building.")
endif()

# --- DetourModKit (AOB scanning, hook manager, config, logger) ---
# Uses the bundled reviewed capacity128 MSVC package, including its licenses.
# An unmodified upstream release has a different adapter capacity and is not a
# substitute for the exact pinned dependency shipped with this source.
# The reviewed MSVC library is stored losslessly compressed, keeping the source
# GitHub-sized. Restore exact pinned bytes only under the chosen build directory.
# An explicit DetourModKit_DIR selects a caller-provided package; prefix-path
# discovery alone cannot silently replace the bundled reviewed dependency.
set(KCD2VR_DMK_RESTORED "${CMAKE_BINARY_DIR}/_deps/DetourModKit")
if(NOT DetourModKit_DIR OR DetourModKit_DIR STREQUAL "${KCD2VR_DMK_RESTORED}/lib/cmake/DetourModKit")
    file(GLOB_RECURSE KCD2VR_DMK_INPUTS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/deps/DetourModKit/*")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${KCD2VR_DMK_INPUTS} "${CMAKE_SOURCE_DIR}/tools/prepare_dmk_dependency.py")
    execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tools/prepare_dmk_dependency.py"
        --package "${CMAKE_SOURCE_DIR}/deps/DetourModKit" --out "${KCD2VR_DMK_RESTORED}"
        COMMAND_ERROR_IS_FATAL ANY)
    set(DetourModKit_DIR "${KCD2VR_DMK_RESTORED}/lib/cmake/DetourModKit" CACHE PATH "Reviewed restored dependency" FORCE)
endif()
find_package(DetourModKit CONFIG REQUIRED)

# --- OpenXR loader ---
# V63.0: vcpkg is optional. Reuse an installed OpenXR package if present;
# otherwise CMake downloads/builds Khronos' loader automatically.
# Imported SDKs may list only Debug/Release, in that order. Keep release-like
# builds off their Debug loader unless the caller explicitly chose a mapping.
if(NOT DEFINED CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO)
    set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)
endif()
option(KCD2VR_SINGLE_BINARY "Link the pinned OpenXR loader into the mod binary" OFF)
if(KCD2VR_SINGLE_BINARY)
    # A proxy filename alone does not remove a shared-loader dependency. Use
    # the pinned source loader even if a shared SDK package is installed.
    set(DYNAMIC_LOADER OFF CACHE BOOL "Static OpenXR loader for one mod binary" FORCE)
else()
    find_package(OpenXR CONFIG QUIET)
endif()
if(NOT TARGET OpenXR::openxr_loader)
    include(FetchContent)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_API_LAYERS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        openxr_sdk_source
        URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.1.62.tar.gz
        URL_HASH SHA512=339ae24974caffa58cabe4e7144a34d5a8c81933ea7f20d142d2af62a19584b466958e3744c45fceb1a33f810a685a64082fa8af20681155a9fbff55e3640131
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(openxr_sdk_source)
    if(TARGET openxr_loader AND NOT TARGET OpenXR::openxr_loader)
        add_library(OpenXR::openxr_loader ALIAS openxr_loader)
    endif()
endif()
if(NOT TARGET OpenXR::openxr_loader)
    message(FATAL_ERROR "OpenXR loader unavailable. Check network access or provide an installed OpenXR package.")
endif()

# V41.1 sanity check: frame-locked stereo requires this translation unit.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/frame_sync.cpp")
    message(FATAL_ERROR "V46.3: src/frame_sync.cpp is missing; extract the complete source package.")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/recursive_view_probe.cpp")
    message(FATAL_ERROR "V46.3: src/recursive_view_probe.cpp is missing; extract the complete source package.")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/native_stereo_outer_lab.cpp")
    message(FATAL_ERROR "V65.03b: src/native_stereo_outer_lab.cpp is missing; F8 native-stereo lab cannot link.")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/action_map_probe.cpp")
    message(FATAL_ERROR "V66.68: src/action_map_probe.cpp missing; passive Shop state observer cannot link.")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/combat_diag_probe.cpp")
    message(FATAL_ERROR "V66.68: src/combat_diag_probe.cpp missing; lazy F3 collision observer cannot link.")
endif()

# One ASI with independently compiled static object modules. No runtime DLL loading.
add_library(KCD2VR SHARED)
include(cmake/module_sources.cmake)

target_include_directories(KCD2VR PRIVATE src)
# C26e: VR menu book skin embedded as a Win32 BITMAP resource (no loose .bmp beside the ASI)
if(WIN32)
    set(KCD2VR_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    # rc.exe accepts forward slashes, so the CMake path is used as is
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/kcd2vr_resources26.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/kcd2vr_resources26.rc" @ONLY)
    target_sources(KCD2VR PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kcd2vr_resources26.rc")
    # R118: the repo KCD2VR.ini is built in as RCDATA (default INI written beside the mod when missing).
    # An edited KCD2VR.ini / menu bitmap must rebuild the resource.
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/kcd2vr_resources26.rc" PROPERTIES
        OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/KCD2VR.ini;${CMAKE_CURRENT_SOURCE_DIR}/assets/MIS_MENU_BOOK.bmp")
endif()

# V64.23 BUILD FIX: MSVC error C1041 ("cannot open program database... if multiple
# CL.EXE write to the same .PDB file, please use /FS"). MSBuild parallelizes cl.exe
# invocations across the many source files in this target by default, and without
# /FS they race writing to the same vc143.pdb. /FS tells the compiler to serialize
# PDB writes through a single MSPDBSRV.EXE instance instead of writing directly,
# which is exactly what parallel compilation of one target needs.
if(MSVC)
    target_compile_options(KCD2VR PRIVATE /FS)
endif()

# D56R1: native-memory probes use __try/__except for hardware faults. clang-cl
# without asynchronous exceptions can emit direct loads outside the SEH scope
# (confirmed in D56 ObjectRttiName). MSVC already covers these SEH regions;
# require the matching model explicitly for the Clang MSVC-compatible build.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    # The native TUs live in OBJECT modules; root PRIVATE flags do not propagate
    # back into those dependencies. Cover both the modules and proxy TU.
    target_compile_options(KCD2VR_compile_contract INTERFACE /EHa)
    target_compile_options(KCD2VR PRIVATE /EHa)
endif()

target_link_libraries(KCD2VR PRIVATE
    DetourModKit::DetourModKit
    OpenXR::openxr_loader
    d3d12
    dxgi
    dxguid
    d3dcompiler
    comctl32
    gdi32
    user32
    shell32
    winmm
    bcrypt
    psapi
)

# A shared OpenXR import remains a runtime dependency in ASI and proxy builds.
# Copy the exact configured target, never a machine-global guessed DLL.
get_target_property(KCD2VR_XR_TYPE32 OpenXR::openxr_loader TYPE)
if(KCD2VR_SINGLE_BINARY AND NOT KCD2VR_XR_TYPE32 STREQUAL "STATIC_LIBRARY")
    message(FATAL_ERROR "Single-binary release requires a static OpenXR loader; got ${KCD2VR_XR_TYPE32}")
endif()
# Prepare current source scripts for either OpenXR linkage mode.
include(cmake/prepare_companion_d39.cmake)
if(KCD2VR_XR_TYPE32 STREQUAL "SHARED_LIBRARY")
add_custom_command(TARGET KCD2VR POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:OpenXR::openxr_loader> $<TARGET_FILE_DIR:KCD2VR>
        COMMENT "C32: copying configured OpenXR runtime dependency beside the output")
endif()
# C63: stage the same reviewed unified package distributed with the source.
# Historical companion/Mods resources remain donor inputs only.
# A Lua-only edit does not relink KCD2VR, so POST_BUILD can leave an old PAK
# beside a current binary. Run the byte-verifying stage for every KCD2VR build,
# including the release helper's explicit --target KCD2VR. Staging is a target
# dependency so it completes before the caller can verify/package the binary;
# it does not force a relink or change the existing binary post-build commands.
add_custom_target(KCD2VR_companion_stage_d39
    COMMAND ${CMAKE_COMMAND}
        "-DSOURCE_ROOT=${KCD2VR_COMPANION_SOURCE_ROOT}"
        "-DTARGET_ROOT=$<TARGET_FILE_DIR:KCD2VR>/companion"
        -P "${CMAKE_SOURCE_DIR}/cmake/stage_companion63.cmake"
    COMMENT "C63: staging reviewed unified companion; no historical split PAKs"
    VERBATIM)
add_dependencies(KCD2VR KCD2VR_companion_stage_d39)

# Build as KCD2VR.asi, not .dll — the ASI loader looks for this extension
set_target_properties(KCD2VR PROPERTIES
    OUTPUT_NAME "KCD2VR"
    SUFFIX ".asi"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release"
)
# C29 release mode: -DKCD2VR_PROXY_DINPUT8=ON builds KCD2VR as dinput8.dll (no asi loader, loads other *.asi itself).
# OpenXR linkage/runtime dependencies come from the selected OpenXR package; proxy mode does not imply static linkage.
option(KCD2VR_PROXY_DINPUT8 "build KCD2VR as dinput8.dll proxy" OFF)
if(KCD2VR_PROXY_DINPUT8 AND WIN32)
    target_sources(KCD2VR PRIVATE src/proxy_dinput8_29.cpp src/proxy_dinput8_29.def)
    target_compile_definitions(KCD2VR PRIVATE KCD2VR_PROXY_DINPUT8=1)
    target_compile_definitions(KCD2VR_compile_contract INTERFACE KCD2VR_PROXY_DINPUT8=1)
    set_target_properties(KCD2VR PROPERTIES OUTPUT_NAME "dinput8" SUFFIX ".dll")
endif()

# Keep runtime config beside the ASI. The optional MIS menu book art was not
# present in the V65.09 source handoff; vr_menu.cpp has a procedural parchment
# fallback, so only add the copy step when the file actually exists.
add_custom_command(TARGET KCD2VR POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        "-DDEFAULT_INI=${CMAKE_CURRENT_SOURCE_DIR}/KCD2VR.ini"
        "-DTARGET_INI=$<TARGET_FILE_DIR:KCD2VR>/KCD2VR.ini"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/seed_ini_if_missing.cmake"
)
# Menu art is always compiled above as BITMAP3101. A pre-existing loose bitmap
# remains a user override at runtime, but is never a release dependency.

target_compile_definitions(KCD2VR PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

if(WIN32)
 if(KCD2VR_PROXY_DINPUT8)
  set(KCD2VR_SETUP_BINARY "dinput8.dll")
 else()
  set(KCD2VR_SETUP_BINARY "KCD2VR.asi")
 endif()
 configure_file("${CMAKE_SOURCE_DIR}/Start KCD2VR.cmd.in" "${CMAKE_BINARY_DIR}/Start KCD2VR.cmd" @ONLY)
 add_custom_command(TARGET KCD2VR POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/Start KCD2VR.cmd" "$<TARGET_FILE_DIR:KCD2VR>/Start KCD2VR.cmd")
endif()
